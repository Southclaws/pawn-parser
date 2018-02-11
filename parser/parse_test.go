package parser

import (
	"testing"

	"github.com/stretchr/testify/assert"

	"github.com/Southclaws/pawn-parser/token"
)

func Test_scanFile(t *testing.T) {
	type args struct {
		fset     *token.FileSet
		filename string
	}
	tests := []struct {
		name       string
		args       args
		wantTokens []token.Token
		wantErr    bool
	}{
		{"ladders", args{token.NewFileSet(), "./tests/ladders.inc"}, []token.Token{}, false},
		{"linegen", args{token.NewFileSet(), "./tests/linegen.inc"}, []token.Token{}, false},
		{"virtual-canvas", args{token.NewFileSet(), "./tests/virtual-canvas.inc"}, []token.Token{}, false},
		{"weapon-data", args{token.NewFileSet(), "./tests/weapon-data.inc"}, []token.Token{}, false},
		{"Zipline", args{token.NewFileSet(), "./tests/Zipline.inc"}, []token.Token{}, false},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			gotTokens, gotErr := scanFile(tt.args.fset, tt.args.filename)
			if tt.wantErr {
				assert.Error(t, gotErr)
			} else {
				assert.NoError(t, gotErr)
			}

			for _, tok := range gotTokens {
				if tok == token.ILLEGAL {
					t.Error("illegal token", tok)
				}
			}
		})
	}
}
