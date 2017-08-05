package main

import (
	"fmt"
	"io/ioutil"

	"github.com/Southclaws/pawn-parser/scanner"
	"github.com/Southclaws/pawn-parser/token"
)

func main() {
	filePath := "sample.pwn"

	var s scanner.Scanner
	fileContents, _ := ioutil.ReadFile(filePath)
	fset := token.NewFileSet()

	file := fset.AddFile(filePath, fset.Base(), len(fileContents))
	s.Init(file, fileContents, nil, scanner.ScanComments)
	depth := 0
	for {
		_, tok, lit := s.Scan()
		if tok == token.EOF {
			break
		}

		if tok.String() == "}" {
			depth--
		}
		for i := 0; i < depth; i++ {
			fmt.Print("\t")
		}
		if tok.String() == "{" {
			depth++
		}

		fmt.Printf("%02d - %s\t%q\n", depth, tok, lit)
	}
}
