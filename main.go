package main

import (
	"fmt"
	"os"

	"github.com/Southclaws/pawn-parser/parse"
)

func main() {
	if len(os.Args) <= 2 {
		fmt.Println("Usage: parse [filename]")
		return
	}

	parse.File(os.Args[1])
}
