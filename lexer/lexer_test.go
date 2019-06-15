package lexer

import (
	"testing"

	"github.com/Southclaws/pawn-parser/token"
)

func TestNextToken(t *testing.T) {
	input := `new five = 5;
new ten = 10;

main() {
	new c = ten + five;
}
`

	tests := []struct {
		expectedType    token.TokenType
		expectedLiteral string
	}{
		{token.NEW, "new"},
		{token.IDENT, "five"},
		{token.ASSIGN, "="},
		{token.INT, "5"},
		{token.SEMICOLON, ";"},
		{token.NEW, "new"},
		{token.IDENT, "ten"},
		{token.ASSIGN, "="},
		{token.INT, "10"},
		{token.SEMICOLON, ";"},
		{token.IDENT, "main"},
		{token.LPAREN, "("},
		{token.RPAREN, ")"},
		{token.LBRACE, "{"},
		{token.NEW, "new"},
		{token.IDENT, "c"},
		{token.ASSIGN, "="},
		{token.IDENT, "ten"},
		{token.ADD, "+"},
		{token.IDENT, "five"},
		{token.SEMICOLON, ";"},
		{token.RBRACE, "}"},
		{token.EOF, ""},
	}

	l := New(input)

	for i, tt := range tests {
		tok := l.NextToken()

		if tok.Type != tt.expectedType {
			t.Errorf("tests[%d] - tokentype wrong. expected=%q, got=%q (%q)",
				i, tt.expectedType, tok.Type, tok.Literal)
		}

		if tok.Literal != tt.expectedLiteral {
			t.Errorf("tests[%d] - literal wrong. expected=%q, got=%q",
				i, tt.expectedLiteral, tok.Literal)
		}
	}
}
