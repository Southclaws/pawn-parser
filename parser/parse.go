package parser

import (
	"fmt"
	"io/ioutil"

	"github.com/Southclaws/pawn-parser/ast"
	"github.com/Southclaws/pawn-parser/scanner"
	"github.com/Southclaws/pawn-parser/token"
)

// File builds a tree from the given file
func File(filename string) (f *ast.File, err error) {
	var sc scanner.Scanner

	fileContents, err := ioutil.ReadFile(filename)
	if err != nil {
		return
	}

	fset := token.NewFileSet()

	file := fset.AddFile(filename, fset.Base(), len(fileContents))
	sc.Init(file, fileContents, nil, scanner.ScanComments)

	depth := 0
	for {
		_, tok, lit := sc.Scan()
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

	return
}
