var is a variable
classname is the name of a class
methodname is the name of a method
str is a string
i is an integer

comment ::= `//` (~newline)* newline
          | `/*` (~`*/`)* `*/`

type ::= `Int` | `Boolean` | `Void` | Built-in types
         classname class type; includes Object and String

comma_exp ::= [exp (`,` exp)*]

primary_exp ::=
  var | str | i | Variables, strings, and integers are     
                  expressions
  `(` exp `)` | Parenthesized expressions
  `this` | Refers to my instance
  `true` | `false` | Booleans
  `println` `(` exp `)` | Prints something to the terminal
  `new` classname `(` comma_exp `)` Creates a new object

call_exp ::= primary_exp (`.` methodname `(` comma_exp `)`)*

mult_exp ::= call_exp ((`*` | `/`) call_exp)*

add_exp ::= mult_exp ((`+` | `-`) mult_exp)*

comp_exp ::= add_exp ['<' add_exp]

eq_exp ::= comp_exp [('==' | '!=') comp_exp)]

exp ::= eq_exp

vardec ::= type var

stmt ::= exp `;` | Expression statements
         vardec `;` | Variable declaration
         var `=` exp `;` | Assignment
         `while` `(` exp `)` stmt | while loops
         `break` `;` | break
         `return` [exp] `;` | return, possibly void
         if with optional else
         `if` `(` exp `)` stmt [`else` stmt] | 
         `{` stmt* `}` Block

comma_vardec ::= [vardec (`,` vardec)*]

methoddef ::= `method` methodname `(` comma_vardec `)` type
              `{` stmt* `}`

constructor ::= `init` `(` comma_vardec `)` `{`
                [`super` `(` comma_exp `)` `;` ]
                stmt*
                `}`
classdef ::= `class` classname [`extends` classname] `{`
             (vardec `;`)*
             constructor
             methoddef*
             `}`

program ::= classdef* stmt+  stmt+ is the entry point
