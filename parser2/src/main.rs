mod parser;

use parser::Lexer;
use parser::Token;
use parser::S;

fn main() {
    let mut lexer = Lexer::new("1 * 2 + 3");
    loop {
        let token = lexer.next();
        println!("Token: \'{:?}\'", token);
        if token == Token::Eof {
            break;
        }
    }

    let s = S::Cons('+', vec![S::Cons('*', vec![S::Atom('1'), S::Atom('2')]), S::Atom('3')]);
    println!("{}", s);

    let s = parser::expr("1 * 2 + 3");
    println!("{}", s);

}