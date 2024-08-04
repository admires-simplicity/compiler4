#include "compiler.h"

// for now this is only compiling a whole source file.
// if we want to make an interpreter we'll need to rework this later.
SyntaxNode *compile(SyntaxNode *node) {
    // expect block
    if (node->token->type != Token::Type::block) {
        std::cerr << "<Compiler> Error: expected block, got " << node->token->literal << "\n";
        return nullptr;
    }

    node->token = new Token(Token::Type::program_block);

    std::vector<SyntaxNode*> function_list;

    SyntaxNode *main_block = new SyntaxNode(new Token(Token::Type::block));

    // gather function definitions into function list,
    // gather other statements into main definition
    // for (auto child : node->children) {
    //     main_block->children.push_back(child);
    // }

    // SyntaxNode *main_fn = new SyntaxNode(new Token("->"), {
    //     new SyntaxNode(new Token(""), {}),
    //     main_block,
    // });

    return node;   
}