package main

import (
	"fmt"
	"os"
	"os/exec"
	"flag"
	"regexp"
	"strings"
	"bytes"
	"time"
	"strconv"
	"errors"
)

const checker = "valgrind"

var (
	flags   = flag.NewFlagSet("memcheck", flag.ContinueOnError)
	cc      = flags.String("cc", "gcc", "specify the compiler used")
	opts    = flags.String("opts", "", "give the options to be run with the program being tested (separated by ':')")
	logging = flags.Bool("log", false, "log the full output to a file")
	debug   = flags.Bool("debug", false, "debug the memcheck program")
	fullout = flags.Bool("fulloutput", false, "show the fulloutput from valgrind")
	stopErr = flag.Bool("stop-on-error", false, "stop the program if an error is found for one command")

	prog string
	verbose bool

	valgrindArgs = []string{"--leak-check=full"}
)

func init() {
	flags.BoolVar(&verbose, "verbose", false, "print full ouput")
	flags.BoolVar(&verbose, "v", false, "")
	flags.StringVar(&prog, "program", "", "give the name of the program to run")
	flags.StringVar(&prog, "p", "", "")

	flags.Parse(os.Args[1:])
	if os.Args[1] == "-h" {
		os.Exit(0)
	}
}

var (
	// Output Cleaning
	pidPat       = regexp.MustCompile("==(.*)== ")
	cleaningRegex = []*regexp.Regexp{
		regexp.MustCompile("==.*== Copyright .*\n"),
		regexp.MustCompile("==.*== Using Valgrind-.*\n"),
	}

	// Heap Summary
	bytesUsed    = regexp.MustCompile("in use at exit: (.*).*\n")
	heapUsage    = regexp.MustCompile("total heap usage: (.*).*\n")
	totalHeapUse = regexp.MustCompile(`total heap usage: (.*) allocs, (.*) frees, (.*) bytes allocated`)
	exitUse      = regexp.MustCompile("in use at exit: (.*) bytes in (.*) blocks")

	// Leak Summary
	definitelyLost = regexp.MustCompile(`definitely lost: (.*) bytes in (.*) blocks`)

	// Stack traces
	stackTrace = regexp.MustCompile(`%s\n(.*)%s\n`)

	// Error Summary
	errorSummary = regexp.MustCompile("ERROR SUMMARY: (.*) errors from .*?\n")

	unnecessaryMsgs = []string{
		"%sMemcheck, a memory error detector\n",
		"%sAll heap blocks were freed -- no leaks are possible\n",
		"%sFor counts of detected and suppressed errors, rerun with: -v\n",
		"%sUse --track-origins=yes to see where uninitialised values come from\n",
	}
)

func dynamicRegex(pattern string, input ...interface{}) *regexp.Regexp {
	return regexp.MustCompile(fmt.Sprintf(pattern, input...))
}

type MemoryBlock struct {
	Bytes  int
	Blocks int
}

func (m *MemoryBlock) IsZero() bool {
	return m.Bytes == 0 && m.Blocks == 0
}

func (m MemoryBlock) String() string {
	return fmt.Sprintf("%d bytes in %d blocks", m.Bytes, m.Blocks)
}

func newMemBlock(bytes, blocks int) MemoryBlock {
	return MemoryBlock{Bytes: bytes, Blocks: blocks}
}

type MemSummary struct {
	Allocs    int
	Frees     int
	Allocated int
	MemAtExit MemoryBlock
	DefLost   MemoryBlock
	Errors    int
}

func (m *MemSummary) LeaksPossible() bool {
	if m.Allocs == m.Frees {
		return true
	}
	return false
}

func ParseOutput(blob []byte) *MemSummary {
	heapUse := totalHeapUse.FindSubmatch(blob)
	if len(heapUse) != 4 {
		fmt.Println("bad match length", len(heapUse))
		fmt.Printf("Output:\n\"%s\"\n", ValgrindOutput(blob))
		os.Exit(1)
	}
	extUse := exitUse.FindSubmatch(blob)
	errsummary := errorSummary.FindSubmatch(blob)
	return &MemSummary{
		Allocs:    atoi(heapUse[1]),
		Frees:     atoi(heapUse[2]),
		Allocated: atoi(heapUse[3]),
		MemAtExit: newMemBlock(atoi(extUse[1]), atoi(extUse[2])),
		Errors:    atoi(errsummary[1]),
	}
}

func command(prog string, args ...string) *exec.Cmd {
	cmd := exec.Command(prog, args...)
	if verbose {
		fmt.Println(strings.Join(cmd.Args, " "))
	}
	return cmd
}

func valgrind(args []string) ([]byte, error) {
	vargs := valgrindArgs
	if *logging {
		vargs = append(valgrindArgs, fmt.Sprintf("--log-file=memcheck-%s.log", time.Now().String()))
	}
	cmd := command(checker, append(vargs, args...)...)
	if *debug {
		fmt.Println("running command:", cmd.Args)
	}
	return cmd.CombinedOutput()
}

func atoi(num []byte) int {
	if bytes.Contains(num, []byte(",")) {
		num = bytes.Replace(num, []byte(","), []byte(""), -1)
	}

	result, err := strconv.Atoi(string(num))
	if err != nil {
		panic(fmt.Sprintf("Integer Parsing Error: %s", err.Error()))
	}
	return result
}

func clean(all, pattern []byte) []byte {
	return bytes.Replace(all, pattern, []byte(""), -1)
}

func ValgrindOutput(output []byte) string {
	if *fullout {
		return string(output)
	}

	var err error
	buf := &bytes.Buffer{}
	pid := pidPat.Find(output)

	for _, pat := range cleaningRegex {
		output = clean(output, pat.Find(output))
	}

	for _, msg := range unnecessaryMsgs {
		output = clean(output, []byte(fmt.Sprintf(msg, pid)))
	}

	for _, line := range dynamicRegex("%s(.*\n)", pid).FindAllSubmatch(output, -1) {
		_, err = buf.Write(line[1])
		if err != nil {
			fmt.Println("Error Cleaning output:", err)
		}
	}
	return buf.String()
}

func LeakSummary(output []byte) string {
	pattern := dynamicRegex("%sLEAK SUMMARY:\n(.*\n){1,5}", pidPat.Find(output))
	out := pattern.FindSubmatch(output)
	if len(out) > 0 {
		return string(out[0])
	}
	return ""
}

func check(program, opt string) error {
	if !verbose {
		fmt.Println("checking for memory leaks...")
	}

	args := []string{program}
	if opt != "" {
		for _, o := range strings.Split(opt, " ") {
			if o != "" {
				args = append(args, o)
			}
		}
	}

	output, _ := valgrind(args)
	if *logging {
		return nil
	}
	summary := ParseOutput(output)

	if verbose {
		fmt.Println(ValgrindOutput(output))
		return nil
	}

	var err error

	if summary.Allocs != summary.Frees || summary.MemAtExit.Bytes != 0 {
		fmt.Print(ValgrindOutput(output))
		print("\n")
		err = errors.New("found memory leak")
	} else if summary.Errors != 0 {
		fmt.Print(ValgrindOutput(output))
		fmt.Printf("Errors: %d\n", summary.Errors)
		print("\n")
		err = errors.New("found valgrind error")
	}

	if summary.Allocs == summary.Frees && summary.MemAtExit.Bytes == 0 {
		fmt.Println("no leaks are possible.")
	}
	return err
}

func main() {
	var err error

	for _, opt := range strings.Split(*opts, ":") {
		err = check(prog, opt)
		if *stopErr && err != nil {
			break
		}
		fmt.Println(strings.Repeat("-", 50))
	}
}
