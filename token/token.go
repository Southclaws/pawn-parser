package token

type TokenType string

type Token struct {
	Type    TokenType
	Literal string
}

//nolint
const (
	ILLEGAL           TokenType = "ILLEGAL"
	EOF                         = "EOF"
	COMMENT                     = "COMMENT"
	IDENT                       = "IDENT"
	LIT_INT                     = "INT"
	LIT_FLOAT                   = "FLOAT"
	LIT_CHAR                    = "CHAR"
	LIT_STRING                  = "STRING"
	OP_ADD                      = "+"
	OP_SUB                      = "-"
	OP_MULTIPLY                 = "*"
	OP_QUO                      = "/"
	OP_MOD                      = "%"
	OP_AND                      = "&"
	OP_OR                       = "|"
	OP_XOR                      = "^"
	OP_SHIFT_L                  = "<<"
	OP_SHIFT_R                  = ">>"
	OP_AND_NOT                  = "&^"
	OP_ADD_ASSIGN               = "+="
	OP_SUB_ASSIGN               = "-="
	OP_MUL_ASSIGN               = "*="
	OP_QUO_ASSIGN               = "/="
	OP_REM_ASSIGN               = "%="
	OP_AND_ASSIGN               = "&="
	OP_OR_ASSIGN                = "|="
	OP_XOR_ASSIGN               = "^="
	OP_SHL_ASSIGN               = "<<="
	OP_SHR_ASSIGN               = ">>="
	OP_AND_NOT_ASSIGN           = "&^="
	OP_LAND                     = "&&"
	OP_LOR                      = "||"
	OP_ARROW                    = "<-"
	OP_INC                      = "++"
	OP_DEC                      = "--"
	OP_EQL                      = "=="
	OP_LSS                      = "<"
	OP_GTR                      = ">"
	OP_ASSIGN                   = "="
	OP_NOT                      = "!"
	OP_NEQ                      = "!="
	OP_LEQ                      = "<="
	OP_GEQ                      = ">="
	ELLIPSIS                    = "..."
	LPAREN                      = "("
	LBRACK                      = "["
	LBRACE                      = "{"
	COMMA                       = ","
	PERIOD                      = "."
	RPAREN                      = ")"
	RBRACK                      = "]"
	RBRACE                      = "}"
	SEMICOLON                   = ";"
	COLON                       = ":"
	DIRECTIVE                   = "#"
	CONST                       = "const"
	STATIC                      = "static"
	STOCK                       = "stock"
	PUBLIC                      = "public"
	NATIVE                      = "native"
	NEW                         = "new"
	IF                          = "if"
	ELSE                        = "else"
	RETURN                      = "return"
	FOR                         = "for"
	CONTINUE                    = "continue"
	BREAK                       = "break"
	GOTO                        = "goto"
	SWITCH                      = "switch"
	CASE                        = "case"
	DEFAULT                     = "default"
	DIR_PRAGMA                  = "pragma"
	DIR_SECTION                 = "section"
	DIR_LINE                    = "line"
	DIR_FILE                    = "file"
	DIR_INCLUDE                 = "include"
	DIR_TRYINCLUDE              = "tryinclude"
	DIR_DEFINE                  = "define"
	DIR_DEFINED                 = "defined"
	DIR_UNDEF                   = "undef"
	DIR_ASSERT                  = "assert"
	DIR_IF                      = "if"
	DIR_ELSEIF                  = "elseif"
	DIR_ELSE                    = "else"
	DIR_ENDIF                   = "endif"
	DIR_ERROR                   = "error"
	DIR_ENDINPUT                = "endinput"
)

var tokens = [...]string{}

/*
char *sc_tokens[] = {
         "assert", "*begin", "break", "case", "char", "const", "continue", "default",
         "defined", "do", "else", "emit", "__emit", "*end", "enum", "exit", "for",
         "forward", "goto", "if", "native", "new", "operator", "public", "return",
         "sizeof", "sleep", "state", "static", "stock", "switch", "tagof", "*then",
         "while",
         "#assert", "#define", "#else", "#elseif", "#emit", "#endif", "#endinput",
         "#endscript", "#error", "#file", "#if", "#include", "#line", "#pragma",
         "#tryinclude", "#undef", "#warning",
         ";", ";", "-integer value-", "-rational value-", "-identifier-",
         "-label-", "-string-",
         "-any value-", "-numeric value-", "-data offset-", "-local variable-",
         "-function-", "-native function-", "-nonnegative integer-"
       };
*/

var keywords = map[string]TokenType{
	"new": NEW,
}

func LookupIdent(ident string) TokenType {
	if tok, ok := keywords[ident]; ok {
		return tok
	}
	return IDENT
}

var t = []string{
	0: "one",
	1: "two",
	2: "three",
}
