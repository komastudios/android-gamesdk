package com.google.android.apps.internal.games.memoryadvice;

import java.util.HashMap;
import java.util.Map;

/**
 * An evaluator to allow expressions with basic arithmetic operators to be evaluated.
 */
class Evaluator {
  private final Map<String, NodeBoolean> cache = new HashMap<>();

  /**
   * Evaluate a boolean expression.
   * @param formula The formula to evaluate.
   * @param lookup A callback to evaluate variable expressions.
   * @return The result of the evaluation.
   * @throws LookupException If the suppled variable cannot be evaluated.
   */
  boolean evaluate(String formula, Lookup lookup) throws LookupException {
    NodeBoolean node = cache.get(formula);
    if (node == null) {
      node = parseBoolean(formula);
      cache.put(formula, node);
    }
    return node.evaluate(lookup);
  }

  /**
   * Compile a boolean expression.
   * @param formula The text formula to compile.
   * @return The root of the expression tree.
   */
  private static NodeBoolean parseBoolean(String formula) {
    formula = formula.trim();
    {
      String[] split = splitFormula(formula, '>');
      if (split.length == 2) {
        return new NodeGreaterThan(parseDouble(split[0]), parseDouble(split[1]));
      }
    }
    {
      String[] split = splitFormula(formula, '<');
      if (split.length == 2) {
        return new NodeLessThan(parseDouble(split[0]), parseDouble(split[1]));
      }
    }
    return null;
  }

  /**
   * Compile a floating point (double) expression.
   * @param formula The text formula to compile.
   * @return The root of the expression tree.
   */
  private static NodeDouble parseDouble(String formula) {
    formula = formula.trim();
    if (formula.charAt(0) == '(' && formula.charAt(formula.length() - 1) == ')') {
      return parseDouble(formula.substring(1, formula.length() - 1));
    }

    {
      String[] split = splitFormula(formula, '+');
      if (split.length == 2) {
        return new NodeAdd(parseDouble(split[0]), parseDouble(split[1]));
      }
    }
    {
      String[] split = splitFormula(formula, '-');
      if (split.length == 2) {
        return new NodeSubtract(parseDouble(split[0]), parseDouble(split[1]));
      }
    }
    {
      String[] split = splitFormula(formula, '*');
      if (split.length == 2) {
        return new NodeMultiply(parseDouble(split[0]), parseDouble(split[1]));
      }
    }

    {
      String[] split = splitFormula(formula, '/');
      if (split.length == 2) {
        return new NodeDivide(parseDouble(split[0]), parseDouble(split[1]));
      }
    }

    try {
      double v = Double.parseDouble(formula);
      return new NodeDoubleLiteral(v);
    } catch (NumberFormatException ex) {
      // Ignored by design.
    }

    return new NodeDoubleParameter(formula);
  }

  /**
   * Split a formula by an operator character, provided it's not inside brackets.
   */
  private static String[] splitFormula(String formula, char splitOn) {
    int bracketCount = 0;
    for (int idx = 0; idx != formula.length(); idx++) {
      char c = formula.charAt(idx);
      if (c == '(') {
        bracketCount++;
      } else if (c == ')') {
        bracketCount--;
      } else if (c == splitOn && bracketCount == 0) {
        return new String[] {formula.substring(0, idx), formula.substring(idx + 1)};
      }
    }
    return new String[] {};
  }
}
