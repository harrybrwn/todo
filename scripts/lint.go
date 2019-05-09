package main

import (
	"bytes"
	"fmt"
	"html/template"
	"os"
	"os/exec"
	"regexp"
	"strconv"
)

const (
	compiler = "gcc"
	inc      = "include"
	lintflag = "-fsyntax-only"
)

var (
	cflags = []string{
		"-Wall",
		"-Werror",
		"-g",
		"-iquote",
		inc,
		"-std=c99",
		// lintflag,
	}
	basepat = `(.*?\.[ch]):(\d+):(\d+): %s`
)

// Error is a gcc error message
type Error struct {
	File      string
	LineNum   int
	CharIndex int
	Msg       string
	IsNote    bool
}

func re(inner string) *regexp.Regexp {
	return regexp.MustCompile(fmt.Sprintf(basepat, inner))
}

// ParseError will parse the gcc output and give an error struct with useful info
func ParseError(msg []byte) *Error {
	parts := re(".*?: (.*$)").FindSubmatch(msg)
	if parts == nil {
		return nil
	}
	line, _ := strconv.Atoi(string(parts[2]))
	char, _ := strconv.Atoi(string(parts[3]))

	return &Error{
		File:      string(parts[1]),
		LineNum:   line,
		CharIndex: char,
		Msg:       string(parts[4]),
		IsNote:    re("note:").Match(msg),
	}
}

var errorTmpl = `{{.File}} - {{.LineNum}}: "{{.Msg}}"`

func (e *Error) String() string {
	buf := &bytes.Buffer{}
	t := template.New("gccError")
	template.Must(t.Parse(errorTmpl))
	t.Execute(buf, e)
	return buf.String()
}

func main() {
	flags := append([]string{lintflag}, os.Args[1:]...)
	output, _ := exec.Command(compiler, flags...).CombinedOutput()

	status := 0
	if len(output) > 0 {
		status = 1
	}
	// fmt.Printf("%s\n", output)

	lines := bytes.Split(output, []byte("\n"))

	var e *Error
	for _, line := range lines {
		e = ParseError(line)
		if e == nil {
			continue
		}
		fmt.Printf("%v\n", e)
	}

	os.Exit(status)
}
