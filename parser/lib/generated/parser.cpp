// A Bison parser, made by GNU Bison 3.5.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





#include "parser.tab.hpp"


// Unqualified %code blocks.
#line 24 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"

#include "parser_driver.hpp"

namespace yy
{

void parser::error(const std::string& e){
    std::cout << "Syntax error has been foud: " << e << std::endl;
    driver->markBadInput();
}

parser::token_type yylex(parser::semantic_type* yylval,                         
                         ParserDriver* driver);

}

#line 62 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 134 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"


  /// Build a parser object.
  parser::parser (ParserDriver *driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value (std::move (that.value))
  {}
#endif

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v)
    : Base (t)
    , value (YY_MOVE (v))
  {}

  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
  }

  // by_type.
  parser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  parser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  parser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
  }


  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_number_type
  parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[+state];
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value))
  {
    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, driver));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 128 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { driver->insert((yystack_[1].value.requestNode)); return 0; }
#line 572 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 3:
#line 129 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { driver->insert((yystack_[1].value.requestNode)); return 0; }
#line 578 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 4:
#line 132 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = new RequestNode((yystack_[0].value.matchExpressionNode)); }
#line 584 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 5:
#line 133 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = new RequestNode((yystack_[0].value.createExpressionNode)); }
#line 590 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 6:
#line 134 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = (yystack_[1].value.requestNode); (yylhs.value.requestNode)->addExpr((yystack_[0].value.matchExpressionNode)); }
#line 596 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 7:
#line 135 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = (yystack_[1].value.requestNode); (yylhs.value.requestNode)->addExpr((yystack_[0].value.setExpressionNode)); }
#line 602 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 8:
#line 136 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = (yystack_[1].value.requestNode); (yylhs.value.requestNode)->addExpr((yystack_[0].value.createExpressionNode)); }
#line 608 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 9:
#line 137 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = (yystack_[1].value.requestNode); (yylhs.value.requestNode)->addExpr((yystack_[0].value.deleteExpressionNode)); }
#line 614 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 10:
#line 138 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.requestNode) = (yystack_[1].value.requestNode); (yylhs.value.requestNode)->addExpr((yystack_[0].value.returnExpressionNode)); }
#line 620 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 11:
#line 141 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                                     { (yylhs.value.matchExpressionNode) = new MatchExpressionNode((yystack_[0].value.variableMatchNode));         }
#line 626 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 12:
#line 142 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                                     { (yylhs.value.matchExpressionNode) = new MatchExpressionNode((yystack_[2].value.variableMatchNode), (yystack_[0].value.variableMatchNode), (yystack_[1].value.relationMatchNode)); }
#line 632 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 13:
#line 143 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                                     { (yylhs.value.matchExpressionNode) = new MatchExpressionNode((yystack_[2].value.variableMatchNode), (yystack_[0].value.variableMatchNode), (yystack_[1].value.relationMatchNode)); }
#line 638 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 14:
#line 146 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                       { (yylhs.value.variableMatchNode) = new VariableFilterMatchNode((yystack_[4].value.name), (yystack_[2].value.name), (yystack_[1].value.predicateNode));  }
#line 644 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 15:
#line 147 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                       { (yylhs.value.variableMatchNode) = new VariablePatternMatchNode((yystack_[6].value.name), (yystack_[4].value.name), (yystack_[2].value.attributeListNode)); }
#line 650 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 16:
#line 148 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                       { (yylhs.value.variableMatchNode) = new VariableMatchNode((yystack_[3].value.name), (yystack_[1].value.name)); }
#line 656 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 17:
#line 149 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                       { (yylhs.value.variableMatchNode) = new VariableMatchNode((yystack_[1].value.name), new std::string("")); }
#line 662 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 18:
#line 152 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                   { (yylhs.value.relationMatchNode) = new RelationMatchNode((yystack_[4].value.name), (yystack_[2].value.name), FORWARD); }
#line 668 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 19:
#line 153 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                   { (yylhs.value.relationMatchNode) = new RelationMatchNode((yystack_[4].value.name), (yystack_[2].value.name), REVERSE); }
#line 674 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 20:
#line 156 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                { (yylhs.value.relationMatchNode) = new RelationMatchNode(new std::string(""), new std::string(""), ANY); }
#line 680 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 21:
#line 159 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                            { (yylhs.value.predicateNode) = new PredicateNode((yystack_[0].value.logicalExpressionNode)); }
#line 686 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 22:
#line 162 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                      { (yylhs.value.logicalExpressionNode) = new AndOperationNode((yystack_[2].value.logicalExpressionNode), (yystack_[0].value.logicalExpressionNode)); }
#line 692 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 23:
#line 163 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                      { (yylhs.value.logicalExpressionNode) = new OrOperationNode((yystack_[2].value.logicalExpressionNode), (yystack_[0].value.logicalExpressionNode));  }
#line 698 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 24:
#line 164 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                      { (yylhs.value.logicalExpressionNode) = new NotOperationNode((yystack_[0].value.logicalExpressionNode));     }
#line 704 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 25:
#line 165 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                      { (yylhs.value.logicalExpressionNode) = new FilterByPassNode((yystack_[0].value.filterNode));     }
#line 710 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 26:
#line 168 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                         { (yylhs.value.filterNode) = new FilterNode((yystack_[2].value.value), (yystack_[0].value.value), LESS);             }
#line 716 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 27:
#line 169 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                         { (yylhs.value.filterNode) = new FilterNode((yystack_[2].value.value), (yystack_[0].value.value), LESS_OR_EQUAL);    }
#line 722 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 28:
#line 170 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                         { (yylhs.value.filterNode) = new FilterNode((yystack_[2].value.value), (yystack_[0].value.value), GREATER);          }
#line 728 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 29:
#line 171 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                         { (yylhs.value.filterNode) = new FilterNode((yystack_[2].value.value), (yystack_[0].value.value), GREATER_OR_EQUAL); }
#line 734 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 30:
#line 172 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                         { (yylhs.value.filterNode) = new FilterNode((yystack_[2].value.value), (yystack_[0].value.value), EQUAL);            }
#line 740 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 31:
#line 173 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                         { (yylhs.value.filterNode) = new FilterNode((yystack_[2].value.value), (yystack_[0].value.value), CONTAINS);         }
#line 746 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 32:
#line 176 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                              { (yylhs.value.setExpressionNode) = new SetExpressionNode(new VariableValueNode((yystack_[4].value.name), (yystack_[2].value.name)), (yystack_[0].value.value)); }
#line 752 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 33:
#line 179 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                       { (yylhs.value.deleteExpressionNode) = new DeleteExpressionNode((yystack_[0].value.name)); }
#line 758 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 34:
#line 182 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                 { (yylhs.value.returnExpressionNode) = (yystack_[2].value.returnExpressionNode); (yylhs.value.returnExpressionNode)->addElement((yystack_[0].value.value));                         }
#line 764 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 35:
#line 183 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                 { (yylhs.value.returnExpressionNode) = new ReturnExpressionNode(); (yylhs.value.returnExpressionNode)->addElement((yystack_[0].value.value)); }
#line 770 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 36:
#line 185 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                      { (yylhs.value.attributeListNode) = (yystack_[0].value.attributeListNode); (yylhs.value.attributeListNode)->addAttribute((yystack_[4].value.name), (yystack_[2].value.value));                      }
#line 776 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 37:
#line 186 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                      { (yylhs.value.attributeListNode) = new AttributeListNode(); (yylhs.value.attributeListNode)->addAttribute((yystack_[2].value.name), (yystack_[0].value.value)); }
#line 782 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 38:
#line 189 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                 { (yylhs.value.createExpressionNode) = new CreateExpressionNode((yystack_[0].value.variableMatchNode)); }
#line 788 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 39:
#line 190 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                                                                               { (yylhs.value.createExpressionNode) = new CreateExpressionNode((yystack_[2].value.variableMatchNode), (yystack_[0].value.variableMatchNode), (yystack_[1].value.relationMatchNode)); }
#line 794 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 40:
#line 193 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                        { (yylhs.value.value) = new VariableValueNode((yystack_[0].value.name), new std::string("")); }
#line 800 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 41:
#line 194 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                        { (yylhs.value.value) = new BoolLiteralNode((yystack_[0].value.boolean));       }
#line 806 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 42:
#line 195 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                        { (yylhs.value.value) = new IntLiteralNode((yystack_[0].value.integer));        }
#line 812 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 43:
#line 196 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                        { (yylhs.value.value) = new FloatLiteralNode((yystack_[0].value.real));      }
#line 818 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 44:
#line 197 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                        { (yylhs.value.value) = new StringLiteralNode((yystack_[0].value.string));     }
#line 824 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;

  case 45:
#line 198 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"
                        { (yylhs.value.value) = new VariableValueNode((yystack_[2].value.name), (yystack_[0].value.name)); }
#line 830 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"
    break;


#line 834 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[+yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yy_error_token_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yy_error_token_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char parser::yypact_ninf_ = -51;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
       5,   -15,   -15,    24,    -2,   -51,   -51,   -18,    -6,     2,
     -51,    20,   -10,    -9,   -51,   -51,   -51,     4,   -51,   -51,
     -51,   -21,     3,    11,   -51,   -15,   -15,   -15,   -51,   -51,
     -51,   -51,    14,   -51,   -51,    21,    20,     7,   -51,    27,
      28,   -51,   -51,   -51,    30,    31,   -51,    10,    47,    48,
     -51,    54,    -1,   -51,    35,    46,    37,    38,    20,    -1,
      36,   -51,     8,    55,    45,   -51,    49,    50,   -51,   -51,
      20,    20,    20,    20,    20,    20,    -1,    -1,    20,    53,
      62,    64,   -51,   -51,   -51,   -51,   -51,   -51,   -51,    75,
      59,   -51,   -51,   -51,    35,   -51
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     4,     5,     0,    11,    38,
       1,     0,     0,     0,     2,     3,     6,    10,     8,     7,
       9,     0,     0,     0,    20,     0,     0,     0,    44,    42,
      43,    41,    40,    35,    33,     0,     0,     0,    17,     0,
       0,    13,    12,    39,     0,     0,    34,     0,     0,     0,
      45,     0,     0,    16,     0,     0,     0,     0,     0,     0,
       0,    25,    21,     0,     0,    14,     0,     0,    32,    24,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28,    29,    26,    27,    30,    31,    22,    23,
      37,    15,    18,    19,     0,    36
  };

  const signed char
  parser::yypgoto_[] =
  {
     -51,   -51,   -51,    82,    17,   -51,    78,   -51,   -11,    84,
     -51,   -51,   -51,   -51,   -50,    -5
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     3,     4,     5,     8,    25,    26,    17,    60,     6,
      19,    20,    61,    55,    62,    64
  };

  const signed char
  parser::yytable_[] =
  {
      33,     1,    37,    11,     2,    12,    13,    38,     1,    69,
      59,     2,     7,    22,    52,    23,    24,    76,    77,     9,
      21,    22,    14,    23,    10,    46,    88,    89,    34,    35,
      36,    15,    39,    28,    29,    30,    31,    32,    53,    44,
      40,    54,    41,    42,    43,    47,    45,    68,    70,    71,
      72,    73,    74,    75,    28,    29,    30,    31,    32,    82,
      83,    84,    85,    86,    87,    48,    49,    90,    50,    51,
      56,    57,    58,    63,    65,    66,    67,    79,    78,    80,
      81,    91,    92,    93,    76,    94,    16,    27,    18,    95
  };

  const signed char
  parser::yycheck_[] =
  {
      11,     3,    23,     5,     6,     7,     8,    28,     3,    59,
      11,     6,    27,    19,     4,    21,    22,     9,    10,     2,
      38,    19,    24,    21,     0,    36,    76,    77,    38,    38,
      26,    33,    29,    34,    35,    36,    37,    38,    28,    25,
      29,    31,    25,    26,    27,    38,    25,    58,    12,    13,
      14,    15,    16,    17,    34,    35,    36,    37,    38,    70,
      71,    72,    73,    74,    75,    38,    38,    78,    38,    38,
      23,    23,    18,    38,    28,    38,    38,    32,    23,    30,
      30,    28,    20,    19,     9,    26,     4,     9,     4,    94
  };

  const signed char
  parser::yystos_[] =
  {
       0,     3,     6,    40,    41,    42,    48,    27,    43,    43,
       0,     5,     7,     8,    24,    33,    42,    46,    48,    49,
      50,    38,    19,    21,    22,    44,    45,    45,    34,    35,
      36,    37,    38,    47,    38,    38,    26,    23,    28,    29,
      29,    43,    43,    43,    25,    25,    47,    38,    38,    38,
      38,    38,     4,    28,    31,    52,    23,    23,    18,    11,
      47,    51,    53,    38,    54,    28,    38,    38,    47,    53,
      12,    13,    14,    15,    16,    17,     9,    10,    23,    32,
      30,    30,    47,    47,    47,    47,    47,    47,    53,    53,
      47,    28,    20,    19,    26,    54
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    39,    40,    40,    41,    41,    41,    41,    41,    41,
      41,    42,    42,    42,    43,    43,    43,    43,    45,    45,
      44,    52,    53,    53,    53,    53,    51,    51,    51,    51,
      51,    51,    49,    50,    46,    46,    54,    54,    48,    48,
      47,    47,    47,    47,    47,    47
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     2,     2,     1,     1,     2,     2,     2,     2,
       2,     2,     4,     4,     6,     8,     5,     3,     7,     7,
       1,     2,     3,     3,     2,     1,     3,     3,     3,     3,
       3,     3,     6,     2,     3,     2,     5,     3,     2,     4,
       1,     1,     1,     1,     1,     3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "$end", "error", "$undefined", "MATCH_KEYWORD", "WHERE_KEYWORD",
  "RETURN_KEYWORD", "CREATE_KEYWORD", "DELETE_KEYWORD", "SET_KEYWORD",
  "AND_KEYWORD", "OR_KEYWORD", "NOT_KEYWORD", "GREATER_CMP",
  "GREATER_OR_EQUAL_CMP", "LESS_CMP", "LESS_OR_EQUAL_CMP", "EQUAL_CMP",
  "CONTAINS_OP", "ASSIGNMENT", "DASH", "RIGHT_ARROW", "LEFT_ARROW",
  "DOUBLE_DASH", "COLON", "SCOLON", "PERIOD", "COMMA", "LPAR", "RPAR",
  "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "END_OF_FILE",
  "STRING_LITERAL", "INT_LITERAL", "FLOAT_LITERAL", "BOOL_LITERAL", "NAME",
  "$accept", "REQUEST", "REQUEST_B", "MATCH_EXPRESSION", "VARIABLE_MATCH",
  "ANY_RELATION_MATCH", "RELATION_MATCH", "RETURN_EXPRESSION", "VALUE",
  "CREATE_EXPRESSION", "SET_EXPRESSION", "DELETE_EXPRESSION", "FILTER",
  "PREDICATE", "LOGICAL_EXPRESSION", "ATTRIBUTE_LIST", YY_NULLPTR
  };


  const unsigned char
  parser::yyrline_[] =
  {
       0,   128,   128,   129,   132,   133,   134,   135,   136,   137,
     138,   141,   142,   143,   146,   147,   148,   149,   152,   153,
     156,   159,   162,   163,   164,   165,   168,   169,   170,   171,
     172,   173,   176,   179,   182,   183,   185,   186,   189,   190,
     193,   194,   195,   196,   197,   198
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const token_number_type
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38
    };
    const int user_token_number_max_ = 293;

    if (t <= 0)
      return yyeof_;
    else if (t <= user_token_number_max_)
      return translate_table[t];
    else
      return yy_undef_token_;
  }

} // yy
#line 1234 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/generated/parser.cpp"

#line 200 "/mnt/c/Users/User/ITMO/low-level-programming/low-level-programming-lab3/parser/lib/parser/parser.y"


namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         ParserDriver* driver)
{
  return driver->yylex(yylval);
}

}
