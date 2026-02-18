#!/bin/bash

# Script de test pour le serveur IRC
# Compile et exécute tous les tests

set -e

echo "╔════════════════════════════════════════╗"
echo "║     IRC SERVER TEST SCRIPT             ║"
echo "╚════════════════════════════════════════╝"
echo ""

# Compilation
echo "🔧 Compilation des tests..."
make test 2>&1 | grep -E "(Built|Building|\[|Error|error)" || true

if [ ! -f "./test_runner" ]; then
    echo "❌ Échec de la compilation des tests"
    exit 1
fi

echo "✓ Compilation réussie"
echo ""

# Exécution des tests
echo "🧪 Exécution des tests..."
echo ""

./test_runner

TEST_RESULT=$?

echo ""
if [ $TEST_RESULT -eq 0 ]; then
    echo "╔════════════════════════════════════════╗"
    echo "║  ✅ TOUS LES TESTS ONT RÉUSSI !       ║"
    echo "╚════════════════════════════════════════╝"
else
    echo "╔════════════════════════════════════════╗"
    echo "║  ❌ CERTAINS TESTS ONT ÉCHOUÉ         ║"
    echo "╚════════════════════════════════════════╝"
fi

exit $TEST_RESULT
