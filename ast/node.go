package ast

import (
	"github.com/Southclaws/pawn-parser/token"
)

// Node types implement the Node interface.
type Node interface {
	Pos() token.Pos // position of first character belonging to the node
	End() token.Pos // position of first character immediately after the node
}

// Expr nodes represent expressions
type Expr interface {
	Node
	exprNode()
}

// Stmt nodes represent statements
type Stmt interface {
	Node
	stmtNode()
}

// Decl nodes represent declarations
type Decl interface {
	Node
	declNode()
}
