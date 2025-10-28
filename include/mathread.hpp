#pragma once

#include <cmath>
#include <optional>
#include <functional>
#include <cassert>
#include <print>
#include <array>
#include <vector>
#include <string>

#if defined(MATH_DEBUG)

#define ASSERT(state) assert(state)
#define PRINT_VALUE(val) std::println("\t\t{}", val)

#else

#define ASSERT(state)
#define PRINT_VALUE(val)

#endif

namespace math_reader {

    enum class token_type {
        error,

        number,
        plus,
        minus,
        slash,
        star,
        left_paren,
        right_paren,
        eof
    };

    struct token {
        std::string lexeme;
        token_type type;
    };

    void print(token t) {
        switch (t.type) {
            case token_type::number:
                std::println("TOKEN: number, {}", t.lexeme); break;
            case token_type::plus:
                std::println("TOKEN: plus, {}", t.lexeme); break;
            case token_type::minus:
                std::println("TOKEN: minus, {}", t.lexeme); break;
            case token_type::slash:
                std::println("TOKEN: slash, {}", t.lexeme); break;
            case token_type::star:
                std::println("TOKEN: star, {}", t.lexeme); break;
            case token_type::left_paren:
                std::println("TOKEN: left_paren, {}", t.lexeme); break;
            case token_type::right_paren:
                std::println("TOKEN: right_paren, {}", t.lexeme); break;
            case token_type::eof:
                std::println("TOKEN: eof, {}", t.lexeme); break;
            case token_type::error:
                std::println("TOKEN: error, {}", t.lexeme); break;
        }
    }

    class lexer {
    public:
        lexer() = delete;
        lexer(const std::string& expression);
        token scan();

    private:
        char advance();
        char peek() const;
        char peek_next() const;

        token make_token(token_type type) const;

        bool is_at_end() const;

        void remove_whitespaces();
        bool is_digit(char c) const;

        token number();

        const std::string m_source;
        int m_start, m_current;
    };

// LEXER IMPL
#if defined(MATH_LEXER_IMPLEMENTATION) || defined(MATH_READER_IMPLEMENTATION)

    lexer::lexer(const std::string& expression)
        : m_source(expression), m_start(0), m_current(0) {}

    bool lexer::is_digit(char c) const {
        return c >= '0' && c <= '9';
    }

    token lexer::make_token(token_type type) const {
        return token { m_source.substr(m_start, m_current - m_start), type };
    }

    bool lexer::is_at_end() const {
        return m_current == m_source.length();
    }

    char lexer::advance() {
        return m_source[m_current++];
    }

    char lexer::peek() const {
        return m_source[m_current];
    }

    char lexer::peek_next() const {
        if (is_at_end()) return '\0';
        return m_source[m_current + 1];
    }

    void lexer::remove_whitespaces() {
        while (!is_at_end()) {
            switch (peek()) {
                case '\t':
                case '\n':
                case '\r':
                case ' ':
                    advance();
                    continue;
                default:
                    return;
            }
        }
    }

    token lexer::number() {
        while (is_digit(peek())) advance();

        if (peek() == '.' && is_digit(peek_next())) {
            advance();

            while (is_digit(peek())) advance();
        }

        return make_token(token_type::number);
    }

    token lexer::scan() {
        remove_whitespaces();
        m_start = m_current;

        if (is_at_end()) return make_token(token_type::eof);

        char c = advance();
        if (is_digit(c))
            return number();

        switch (c) {
            case '+':
                return make_token(token_type::plus);
            case '-':
                return make_token(token_type::minus);
            case '/':
                return make_token(token_type::slash);
            case '*':
                return make_token(token_type::star);
            case '(':
                return make_token(token_type::left_paren);
            case ')':
                return make_token(token_type::right_paren);
        };

        return token { "Unexpected character", token_type::error };
    }

#endif

    enum class opcode {
        load = 0,       // load number in register
        negate,         // in out
        add,            // in1 in2 out
        subtract,       // in1 in2 out
        divide,         // in1 in2 out
        multiply,       // in1 in2 out
    };
    constexpr int len_opcodes = 6;
    using byte = unsigned char;

    struct chunk {
        std::vector<byte> program;
        std::vector<double> constants;
    };

    class virtual_machine {
        public:
            virtual_machine(chunk& chunk);
            void run();
            double result();

        private:
            bool execute_instruction();
            void disassemble_instruction(int offset);

            opcode advance();
            byte read_byte();

            double pop();
            void push(double val);

            opcode convert_to_opcode(byte b);

            std::vector<double> m_stack;
            chunk m_chunk;
            int m_pc;
    };

#if defined(MATH_VM_IMPLEMENTATION) || defined(MATH_READER_IMPLEMENTATION)

    virtual_machine::virtual_machine(chunk& chunk) 
        : m_chunk(std::move(chunk)), m_pc(0) { }

    opcode virtual_machine::convert_to_opcode(byte b) {
        ASSERT(b < len_opcodes && "It is not an opcode.");
        return static_cast<opcode>(b);
    }

    void virtual_machine::run() {
        bool still_running = true;
        while (still_running) {
            still_running = execute_instruction();
        }
    }

    double virtual_machine::result() {
        return m_stack[0];
    }

    double virtual_machine::pop() {
        double val = m_stack.back();
        PRINT_VALUE(val);
        m_stack.pop_back();
        return val;
    }

    void virtual_machine::push(double val) {
        PRINT_VALUE(val);
        m_stack.push_back(val);
    }

    bool virtual_machine::execute_instruction() {
        if (m_pc >= m_chunk.program.size()) {
            return false;
        }

#if defined(MATH_DEBUG)
        disassemble_instruction(m_pc);
#endif

        switch (advance()) {
            case opcode::load: {
                push(m_chunk.constants[read_byte()]);
                break;
            }
            case opcode::add: {
                double val2 = pop();
                double val1 = pop();
                push(val1 + val2);
                break;
            }
            case opcode::subtract: {
                double val2 = pop();
                double val1 = pop();
                push(val1 - val2);
                break;
            }
            case opcode::multiply: {
                double val2 = pop();
                double val1 = pop();
                push(val1 * val2);
                break;
            }
            case opcode::divide: {
                double val2 = pop();
                double val1 = pop();
                push(val1 / val2);
                break;
            }
            default:
                break;
        }

        return true;
    }

    void virtual_machine::disassemble_instruction(int offset) {
        switch (static_cast<opcode>(m_chunk.program[offset])) {
            case opcode::load: {
                std::println("load");
                break;
            }
            case opcode::negate: {
                std::println("negate");
                break;
            }
            case opcode::add: {
                std::println("add");
                break;
            }
            case opcode::subtract: {
                std::println("subtract");
                break;
            }
            case opcode::divide: {
                std::println("divide");
                break;
            }
            case opcode::multiply: {
                std::println("multiply");
                break;
            }
        }
    }

    opcode virtual_machine::advance() {
        return convert_to_opcode(m_chunk.program[m_pc++]);
    }

    byte virtual_machine::read_byte() {
        return m_chunk.program[m_pc++];
    }

#endif

    enum class precendence {
        none,
        term,        // + -
        factor,      // * /
        unary,       // -
        primary      // number
    };

    struct binding {
        precendence prec;
        std::optional<std::function<void()>> prefix;
        std::optional<std::function<void()>> infix;
    };

    class compiler {
    public:
        compiler(const std::string& expression);

        chunk compile();
    private:
        void advance();
        void consume(token_type type, std::string_view msg);

        binding get(token token);
        void error(token token, std::string_view msg);

        void expression_binding_power(precendence prec);
        void expression();
        void grouping();
        void number();
        void binary();

        void emit_byte(byte byte);
        void emit_byte(opcode opcode);
        void emit_constant(double value);

        token m_current;
        token m_previous;

        chunk m_chunk;
        lexer m_lexer;
    };

    void interpret(const std::string& expression);

#if defined(MATH_COMPILER_IMPLEMENTATION) || defined(MATH_READER_IMPLEMENTATION)

    compiler::compiler(const std::string& expression)
        : m_lexer(expression), m_chunk(), m_previous(), m_current() {}

    chunk compiler::compile() {
        advance();
        expression_binding_power(precendence::term);
        return m_chunk;
    }

    binding compiler::get(token token) {
#define BIND(fn) std::bind(&compiler::fn, this)
        switch (token.type) {
            case token_type::error:
                return { precendence::none, std::nullopt, std::nullopt };
            case token_type::number:
                return { precendence::none, BIND(number), std::nullopt };
            case token_type::plus:
                return { precendence::term, std::nullopt, BIND(binary) };
            case token_type::minus:
                return { precendence::term, std::nullopt, BIND(binary) };
            case token_type::slash:
                return { precendence::factor, std::nullopt, BIND(binary) };
            case token_type::star:
                return { precendence::factor, std::nullopt, BIND(binary) };
            case token_type::left_paren:
                return { precendence::none, BIND(grouping), std::nullopt };
            case token_type::right_paren:
                return { precendence::none, std::nullopt, std::nullopt };
            case token_type::eof:
                return { precendence::none, std::nullopt, std::nullopt };
        }

        return { precendence::none, std::nullopt, std::nullopt };
#undef BIND
    }

    void compiler::expression() {
        expression_binding_power(precendence::term);
    }

    void compiler::expression_binding_power(precendence prec) {
        advance();
        auto prefix_rule = get(m_previous).prefix;
        if (prefix_rule == std::nullopt) {
            error(m_previous, "prefix rule is nullopt");
            return;
        }

        prefix_rule.value()();

        auto binding = get(m_current);
        while (prec <= binding.prec) {
            advance();
            binding.infix.value()();
            binding = get(m_current);
        }
    }

    void compiler::grouping() {
        expression();
        consume(token_type::right_paren, "failed to consume right paren");
    }

    void compiler::number() {
        emit_constant(std::stod(m_previous.lexeme));
    }

    void compiler::binary() {
        token_type type = m_previous.type;
        expression_binding_power(static_cast<precendence>(
                    static_cast<int>(
                        get(m_previous).prec) + 1
                    ));

        switch (type) {
            case token_type::plus:
                emit_byte(opcode::add); break;
            case token_type::minus:
                emit_byte(opcode::subtract); break;
            case token_type::slash:
                emit_byte(opcode::divide); break;
            case token_type::star:
                emit_byte(opcode::multiply); break;
            default:
                return;
        }
    }

    void compiler::emit_byte(byte byte) {
        m_chunk.program.push_back(byte);
    }

    void compiler::emit_byte(opcode opcode) {
        m_chunk.program.push_back(static_cast<byte>(opcode));
    }

    void compiler::emit_constant(double value) {
        m_chunk.constants.push_back(value);
        emit_byte(opcode::load);
        emit_byte(m_chunk.constants.size() - 1);
    }

    void compiler::error(token token, std::string_view msg) {
        std::println("Error at {}, caused by {}", token.lexeme, msg);
    }

    void compiler::advance() {
        m_previous = m_current;

        while (true) {
            m_current = m_lexer.scan();
            if (m_current.type != token_type::error) break;

            error(m_current, "error token");
        }
    }

    void compiler::consume(token_type type, std::string_view msg) {
        if (m_current.type == type) {
            advance();
            return;
        }

        error(m_current, msg);
    }

    void interpret(const std::string& expression, double& result) {
        compiler compiler(expression);
        auto chunk = compiler.compile();

        virtual_machine vm(chunk);
        vm.run();

        result = vm.result();
    }

#endif

}
