#pragma once
#include <vector>
#include <string>
#include <map>

#include "lexer.h"
#include "types.h"
#include "options.h"
#include "bimap.h"

class SyntaxNode;
class ValueNode;
class ApplyNode;
class BlockNode;
class ProgramBlockNode;
class LetNode;
class FnDefNode;

class SyntaxNodeVisitor;

class SyntaxNode {
protected:
  Type *type = new AtomicType(TypeSet::get_id("unassigned type"));
  virtual std::string to_string_internal() = 0;
public:
  virtual void accept(SyntaxNodeVisitor &v) = 0;
  virtual inline std::string name() = 0;
  virtual std::string to_string();
  const Type *get_type() { return type; }
};

class StmtNode : public SyntaxNode {
protected:
  std::string to_string_internal() override;
public:
  SyntaxNode *expr;
  StmtNode() {}
  StmtNode(SyntaxNode *expr) : expr(expr) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
};

class ReturnNode : public SyntaxNode {
protected:
  std::string to_string_internal() override;
public:
  SyntaxNode *expr;
  ReturnNode(SyntaxNode *expr) : expr(expr) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
};

class ValueNode : public SyntaxNode {
protected:
  std::string to_string_internal() override;
public:
  Token *token;
  ValueNode(Type *type, Token *token) : token(token) { this->type = type; }
  ValueNode(Token* token) : token(token) {}
  ValueNode(std::string literal) : token(new Token(literal)) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
};

class ApplyNode : public SyntaxNode {
protected:
  std::string to_string_internal() override;
public:
  SyntaxNode *fn;
  std::vector<SyntaxNode *> args;
  ApplyNode(SyntaxNode *fn, std::vector<SyntaxNode *> args) : fn(fn), args(args) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;

  SyntaxNode *operator[](int i);
};

class BlockNode : public SyntaxNode {
protected:
  std::string to_string_internal() override;
public:
  std::vector<StmtNode *> children;

  BlockNode() {}
  BlockNode(std::vector<StmtNode *> children) : children(children) {}
  BlockNode(std::vector<SyntaxNode *> children);

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;

  SyntaxNode *operator[](int i);
};

class ProgramBlockNode : public BlockNode {
public:
  void accept(SyntaxNodeVisitor &v) override;
};

class LetNode : public SyntaxNode {
protected:
  std::string to_string_internal() override;
public:
  ValueNode *ident;
  ValueNode *value;
  LetNode(ValueNode *ident, ValueNode *value) : ident(ident), value(value) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
};

// TODO:
// does this properly handle expression functions?
class FnDefNode : public StmtNode {
protected:
  std::string to_string_internal() override;
public:
  ValueNode *ident;
  std::vector<SyntaxNode*> args;
  BlockNode *block;
  FnDefNode(ValueNode *ident, std::vector<SyntaxNode*> args, BlockNode *block) : ident(ident), args(args), block(block) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
};

class SyntaxNodeVisitor {
public:
  virtual void visit(StmtNode *node) = 0;
  virtual void visit(ReturnNode *node) = 0;
  virtual void visit(ValueNode *node) = 0;
  virtual void visit(ApplyNode *node) = 0;
  virtual void visit(BlockNode *node) = 0;
  virtual void visit(ProgramBlockNode *node) = 0;
  virtual void visit(LetNode *node) = 0;
  virtual void visit(FnDefNode *node) = 0;
  virtual ~SyntaxNodeVisitor() = default;
};