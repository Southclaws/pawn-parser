// Package token defines constants and operations for Pawn's tokens.
package token

import (
	"strconv"
)

// Token represents a single token from the language
type Token int

// nolint
const (
	ILLEGAL        Token = iota // Special tokens
	EOF                         // end of file
	COMMENT                     // comment
	literal_beg                 // Identifiers and primitive type literals
	IDENT                       // main - any non-keyword, non-operator token
	INT                         // 12345, or 12_345
	FLOAT                       // 123.45
	CHAR                        // 'a'
	STRING                      // "abc"
	literal_end                 //
	operator_beg                // Operators and delimiters
	ADD                         // +
	SUB                         // -
	MUL                         // *
	QUO                         // /
	REM                         // %
	AND                         // &
	OR                          // |
	XOR                         // ^
	SHL                         // <<
	SHR                         // >>
	AND_NOT                     // &^
	ADD_ASSIGN                  // +=
	SUB_ASSIGN                  // -=
	MUL_ASSIGN                  // *=
	QUO_ASSIGN                  // /=
	REM_ASSIGN                  // %=
	AND_ASSIGN                  // &=
	OR_ASSIGN                   // |=
	XOR_ASSIGN                  // ^=
	SHL_ASSIGN                  // <<=
	SHR_ASSIGN                  // >>=
	AND_NOT_ASSIGN              // &^=
	LAND                        // &&
	LOR                         // ||
	ARROW                       // <-
	INC                         // ++
	DEC                         // --
	EQL                         // ==
	LSS                         // <
	GTR                         // >
	ASSIGN                      // =
	NOT                         // !
	NEQ                         // !=
	LEQ                         // <=
	GEQ                         // >=
	ELLIPSIS                    // ...
	LPAREN                      // (
	LBRACK                      // [
	LBRACE                      // {
	COMMA                       // ,
	PERIOD                      // .
	RPAREN                      // )
	RBRACK                      // ]
	RBRACE                      // }
	SEMICOLON                   // ;
	COLON                       // :
	DIRECTIVE                   // #
	operator_end                //
	keyword_beg                 // Keywords - reserved Pawn words
	CONST                       // const
	STATIC                      // static
	STOCK                       // stock
	PUBLIC                      // public
	NATIVE                      // native
	NEW                         // new
	IF                          // if
	ELSE                        // else
	RETURN                      // return
	FOR                         // for
	CONTINUE                    // continue
	BREAK                       // break
	GOTO                        // goto
	SWITCH                      // switch
	CASE                        // case
	DEFAULT                     // default
	keyword_end                 //
	directive_beg               // directives such as #include #define etc.
	DPRAGMA                     // #pragma
	DSECTION                    // #section
	DLINE                       // #line
	DFILE                       // #file
	DINCLUDE                    // #include <somelib>
	DTRYINCLUDE                 // #tryinclude <somelib>
	DDEFINE                     // #define SOME_CONST
	DDEFINED                    // #if defined SOME_CONST
	DUNDEF                      // #undef SOME_CONST
	DASSERT                     // #assert
	DIF                         // #if SOME_CONST
	DELSEIF                     // #elseif SOME_CONST
	DELSE                       // #else
	DENDIF                      // #endif
	DERROR                      // #error
	DENDINPUT                   // #endinput
	directive_end
)

var tokens = [...]string{
	ILLEGAL:        "ILLEGAL",
	EOF:            "EOF",
	COMMENT:        "COMMENT",
	IDENT:          "IDENT",
	INT:            "INT",
	FLOAT:          "FLOAT",
	CHAR:           "CHAR",
	STRING:         "STRING",
	ADD:            "+",
	SUB:            "-",
	MUL:            "*",
	QUO:            "/",
	REM:            "%",
	AND:            "&",
	OR:             "|",
	XOR:            "^",
	SHL:            "<<",
	SHR:            ">>",
	AND_NOT:        "&^",
	ADD_ASSIGN:     "+=",
	SUB_ASSIGN:     "-=",
	MUL_ASSIGN:     "*=",
	QUO_ASSIGN:     "/=",
	REM_ASSIGN:     "%=",
	AND_ASSIGN:     "&=",
	OR_ASSIGN:      "|=",
	XOR_ASSIGN:     "^=",
	SHL_ASSIGN:     "<<=",
	SHR_ASSIGN:     ">>=",
	AND_NOT_ASSIGN: "&^=",
	LAND:           "&&",
	LOR:            "||",
	ARROW:          "<-",
	INC:            "++",
	DEC:            "--",
	EQL:            "==",
	LSS:            "<",
	GTR:            ">",
	ASSIGN:         "=",
	NOT:            "!",
	NEQ:            "!=",
	LEQ:            "<=",
	GEQ:            ">=",
	ELLIPSIS:       "...",
	LPAREN:         "(",
	LBRACK:         "[",
	LBRACE:         "{",
	COMMA:          ",",
	PERIOD:         ".",
	RPAREN:         ")",
	RBRACK:         "]",
	RBRACE:         "}",
	SEMICOLON:      ";",
	COLON:          ":",
	DIRECTIVE:      "#",
	CONST:          "const",
	STATIC:         "static",
	STOCK:          "stock",
	PUBLIC:         "public",
	NATIVE:         "native",
	NEW:            "new",
	IF:             "if",
	ELSE:           "else",
	RETURN:         "return",
	FOR:            "for",
	CONTINUE:       "continue",
	BREAK:          "break",
	GOTO:           "goto",
	SWITCH:         "switch",
	CASE:           "case",
	DEFAULT:        "default",
	DPRAGMA:        "pragma",
	DSECTION:       "section",
	DLINE:          "line",
	DFILE:          "file",
	DINCLUDE:       "include",
	DTRYINCLUDE:    "tryinclude",
	DDEFINE:        "define",
	DDEFINED:       "defined",
	DUNDEF:         "undef",
	DASSERT:        "assert",
	DIF:            "if",
	DELSEIF:        "elseif",
	DELSE:          "else",
	DENDIF:         "endif",
	DERROR:         "error",
	DENDINPUT:      "endinput",
}

// build a lookup table for keywords and directives
var keywords map[string]Token
var directives map[string]Token

func init() {
	keywords = make(map[string]Token)
	for i := keyword_beg + 1; i < keyword_end; i++ {
		keywords[tokens[i]] = i
	}
	directives = make(map[string]Token)
	for i := directive_beg + 1; i < directive_end; i++ {
		directives[tokens[i]] = i
	}
}

// String returns the string corresponding to the token tok. For operators, delimiters, and keywords
// the string is the actual token character sequence (e.g., for the token ADD, the string is "+").
// For all other tokens the string corresponds to the token constant name (e.g. for the token IDENT,
// the string is "IDENT").
func (tok Token) String() string {
	s := ""
	if 0 <= tok && tok < Token(len(tokens)) {
		s = tokens[tok]
	}
	if s == "" {
		s = "token(" + strconv.Itoa(int(tok)) + ")"
	}
	return s
}

// A set of constants for precedence-based expression parsing. Non-operators have lowest precedence,
// followed by operators starting with precedence 1 up to unary operators. The highest precedence
// serves as "catch-all" precedence for selector, indexing, and other operator and delimiter tokens.
const (
	LowestPrec  = 0 // non-operators
	UnaryPrec   = 6
	HighestPrec = 7
)

// Precedence returns the operator precedence of the binary
// operator tok. If tok is not a binary operator, the result
// is LowestPrecedence.
func (tok Token) Precedence() int {
	switch tok {
	case LOR:
		return 1
	case LAND:
		return 2
	case EQL, NEQ, LSS, LEQ, GTR, GEQ:
		return 3
	case ADD, SUB, OR, XOR:
		return 4
	case MUL, QUO, REM, SHL, SHR, AND, AND_NOT:
		return 5
	}
	return LowestPrec
}

// Lookup maps an identifier to its keyword or directive token or IDENT (if not a keyword).
func Lookup(ident string) Token {
	if tok, is := keywords[ident]; is {
		return tok
	}
	if tok, is := directives[ident]; is {
		return tok
	}
	return IDENT
}

// IsLiteral returns true for tokens corresponding to identifiers and basic type literals.
func (tok Token) IsLiteral() bool { return literal_beg < tok && tok < literal_end }

// IsOperator returns true for tokens corresponding to operators and delimiters
func (tok Token) IsOperator() bool { return operator_beg < tok && tok < operator_end }

// IsKeyword returns true for tokens corresponding to keywords
func (tok Token) IsKeyword() bool { return keyword_beg < tok && tok < keyword_end }

// IsDirective returns true for tokens corresponding to directives
func (tok Token) IsDirective() bool { return directive_beg < tok && tok < directive_end }
