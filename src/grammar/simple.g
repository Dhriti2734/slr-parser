Program   -> StmtList
StmtList  -> StmtList Stmt
StmtList  -> Stmt
Stmt      -> int id = Expr ;
Stmt      -> id = Expr ;
Stmt      -> if ( Expr ) { StmtList }
Stmt      -> while ( Expr ) { StmtList }
Stmt      -> return Expr ;
Expr      -> Expr + Term
Expr      -> Expr - Term
Expr      -> Term
Term      -> id
Term      -> num