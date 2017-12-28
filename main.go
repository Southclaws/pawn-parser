package main

import (
	"fmt"
	"os"

	"github.com/Southclaws/pawn-parser/parser"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: parse [filename]")
		return
	}

	// interesting: /usr/local/go/src/go/parser/interface.go

	parser.File(os.Args[1])
}
