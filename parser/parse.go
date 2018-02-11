package parser

import (
	"fmt"
	"io/ioutil"

	"github.com/Southclaws/pawn-parser/ast"
	"github.com/Southclaws/pawn-parser/scanner"
	"github.com/Southclaws/pawn-parser/token"
)

// File builds a tree from the given file
func File(filename string, includePaths []string) (f *ast.File, err error) {
	fset := token.NewFileSet()
	tokens, err := scanFile(fset, filename)
	if err != nil {
		return
	}

	fmt.Println(tokens)

	return
}

func scanFile(fset *token.FileSet, filename string) (tokens []token.Token, err error) {
	fileContents, err := ioutil.ReadFile(filename)
	if err != nil {
		return
	}
	file := fset.AddFile(filename, fset.Base(), len(fileContents))

	var scan scanner.Scanner
	scan.Init(file, fileContents, nil, scanner.ScanComments)

	for {
		_, tok, _ := scan.Scan()
		if tok == token.EOF {
			break
		}
		tokens = append(tokens, tok)
	}

	return
}
