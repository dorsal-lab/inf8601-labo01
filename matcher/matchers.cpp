/** \file matchers.cpp
 * \brief Clang AST Matchers for automatic correction :: implementation
 *
 * \author Sébastien Darche <sebastien.darche@polymtl.ca>
 */

#include <array>

#include "matchers.hpp"

using namespace clang;
using namespace clang::ast_matchers;

// Variants that require a lambda

std::set<unsigned int> requires_lambdas = {2u, 4u, 5u, 8u, 9u};

// AST matchers

clang::ast_matchers::StatementMatcher filterWithLambdaMatcher =
    cxxConstructExpr(
        hasDeclaration(namedDecl(hasName("filter_t"))),
        hasArgument(
            1,
            expr(anyOf(declRefExpr(to(
                           varDecl(hasInitializer(lambdaExpr())).bind("decl"))),
                       lambdaExpr()))
                .bind("expr"))) // declRefExpr(to(cxxRecordDecl()))
        .bind("filterLambda");

clang::ast_matchers::StatementMatcher filterWithFunctorMatcher =
    cxxConstructExpr(
        hasDeclaration(namedDecl(hasName("filter_t"))),
        hasArgument(
            1, expr(anyOf(cxxConstructExpr(),
                          declRefExpr(
                              to(varDecl(hasInitializer(cxxConstructExpr()))
                                     .bind("decl")))))
                   .bind("expr"))) // declRefExpr(to(cxxRecordDecl()))
        .bind("filterFunctor");

// FilterCallback

bool FilterCallback::usesLambdas() const {
    return lambda_count > functor_count;
}

int FilterCallback::assertVariant(unsigned int variant) const {
    // Checks that it both expects lambdas and uses them, or neither
    if (requires_lambdas.contains(variant) == usesLambdas()) {
        return 0;
    }

    return -1;
}

void FilterCallback::run(
    const clang::ast_matchers::MatchFinder::MatchResult& result) {
    if (const auto* match =
            result.Nodes.getNodeAs<CXXConstructExpr>("filterLambda")) {

        if (const auto* decl_ref = result.Nodes.getNodeAs<Decl>("decl")) {
            decl_ref->dump();
        }

        llvm::errs() << "LAMBDA : ";
        match->dump();
        ++lambda_count;
    }

    if (const auto* match =
            result.Nodes.getNodeAs<CXXConstructExpr>("filterFunctor")) {

        llvm::errs() << "FUNCTOR : ";
        match->dump();
        ++functor_count;
    }

    llvm::errs() << '\n';
}

