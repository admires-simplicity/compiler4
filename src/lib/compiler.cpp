#include <cassert>
#include "compiler.h"

bool is_fn_decl(SyntaxNode *node) {
    if (node == nullptr) return false;
    if (node->token == nullptr) return false;
    if (node->token->literal == ";") node = node->children[0];
    if (node->type == SyntaxNode::Type::literal && node->token->literal == "=") node = node->children[0];
    else return false;
    if (node->token->literal == "->") return true;
    else return false;
}

bool last_to_return(SyntaxNode *node) {
    assert(node->type == SyntaxNode::Type::block);
    bool make_stmt = false;
    SyntaxNode **dest = &node->children[node->children.size()-1];
    SyntaxNode *last_stmt = *dest; //node->children[node->children.size()-1];

    // TODO: this only works if last_stmt is actually an expression statement (in C)
    // e.g. this won't work for blocks or various other AST types like var defns

    if (last_stmt->token->literal != ";") {
        *dest = new SyntaxNode(";", {last_stmt});
        last_stmt = *dest;
    }

    last_stmt->children = { new SyntaxNode("return", last_stmt->children) }; // TODO: is this overkill? Shouldn't we only ever need last_stmt->child[0] ?

    return true;
}

SyntaxNode *get_block_in_fn_defn(SyntaxNode *node) {
    if (node->token->literal == ";") node = node->children[0];
    node = node->children[1]; // (= (-> ...) <block>) -> <block>
    return node;
}


// for now this is only compiling a whole source file.
// if we want to make an interpreter we'll need to rework this later.
SyntaxNode *compile(SyntaxNode *node) {
    // expect block
    if (node->type != SyntaxNode::Type::block) {
        std::cerr << "<Compiler> Error: expected block, got " << node->token->literal << "\n";
        return nullptr;
    }

    //node->token = new Token(Token::Type::program_block);

    std::vector<SyntaxNode *> function_decls;
    
    SyntaxNode *main_block = new SyntaxNode(SyntaxNode::Type::block);
    SyntaxNode *main_fn =
        new SyntaxNode(";", {
            new SyntaxNode("=", {
                new SyntaxNode("->", {
                    new SyntaxNode(SyntaxNode::Type::apply, {new SyntaxNode("main")}),
                    new SyntaxNode("int")}),
                main_block})});

    SyntaxNode *program = new SyntaxNode(SyntaxNode::Type::program_block);

    for (auto child : node->children) {
        if (is_fn_decl(child)) {
            last_to_return(get_block_in_fn_defn(child)); // TODO: this only works assuming fn is actually a block
            program->children.push_back(child);
        }
        // TODO: if is var, put it in global vars
        else {
            main_block->children.push_back(child);
        }
    }

    program->children.push_back(main_fn);

    



    // std::vector<SyntaxNode*> function_list;

    // SyntaxNode *main_block = new SyntaxNode(new Token(Token::Type::block));

    // gather function definitions into function list,
    // gather other statements into main definition
    // for (auto child : node->children) {
    //     main_block->children.push_back(child);
    // }

    // SyntaxNode *main_fn = new SyntaxNode(new Token("->"), {
    //     new SyntaxNode(new Token(""), {}),
    //     main_block,
    // });

    // TODO: delete node
    return program;   
}