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
public:
  virtual void accept(SyntaxNodeVisitor &v) = 0;
  virtual inline std::string name() = 0;
  virtual std::string to_string() = 0;
};

class ValueNode : public SyntaxNode {
public:
  Type *type = new AtomicType(TypeSet::get_id("unassigned type"));
  Token *token;
  ValueNode(Type *type, Token *token) : type(type), token(token) {}
  ValueNode(Token* token) : token(token) {}
  ValueNode(std::string literal) : token(new Token(literal)) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
  std::string to_string() override;
};

class ApplyNode : public SyntaxNode {
public:
  SyntaxNode *fn;
  std::vector<SyntaxNode *> args;
  ApplyNode(SyntaxNode *fn, std::vector<SyntaxNode *> args) : fn(fn), args(args) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
  std::string to_string() override;

  SyntaxNode *operator[](int i);
};

class BlockNode : public SyntaxNode {
public:
  std::vector<SyntaxNode *> children;

  BlockNode() {}
  BlockNode(std::vector<SyntaxNode *> children) : children(children) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
  std::string to_string() override;

  SyntaxNode *operator[](int i);
};

class ProgramBlockNode : public BlockNode {
public:
  void accept(SyntaxNodeVisitor &v) override;
};

class LetNode : public SyntaxNode {
public:
  ValueNode *ident;
  ValueNode *value;
  LetNode(ValueNode *ident, ValueNode *value) : ident(ident), value(value) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
  std::string to_string() override;
};

// TODO:
// does this properly handle expression functions?
class FnDefNode : public SyntaxNode {
public:
  ValueNode *ident;
  std::vector<SyntaxNode*> args;
  BlockNode *block;
  FnDefNode(ValueNode *ident, std::vector<SyntaxNode*> args, BlockNode *block) : ident(ident), args(args), block(block) {}

  void accept(SyntaxNodeVisitor &v) override;
  inline std::string name() override;
  std::string to_string() override;
};

class SyntaxNodeVisitor {
public:
  virtual void visit(ValueNode *node) = 0;
  virtual void visit(ApplyNode *node) = 0;
  virtual void visit(BlockNode *node) = 0;
  virtual void visit(ProgramBlockNode *node) = 0;
  virtual void visit(LetNode *node) = 0;
  virtual void visit(FnDefNode *node) = 0;
  virtual ~SyntaxNodeVisitor() = default;
};