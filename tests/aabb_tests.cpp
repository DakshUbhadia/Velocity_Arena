// aabb_tests.cpp
//
// Dependency-free AABB correctness tests.
// No external testing framework required.
// Wire through CTest: add_test(NAME AABBTests COMMAND aabb_tests)
//
// Tests:
//  1. Two clearly separated boxes (X gap)   => no intersection
//  2. Partial overlap on all axes            => intersection
//  3. One box inside another                 => intersection
//  4. Face touching (shared face plane)      => intersection
//  5. Edge touching                          => intersection
//  6. Corner touching                        => intersection
//  7. Separation on X only                   => no intersection
//  8. Separation on Y only                   => no intersection
//  9. Separation on Z only                   => no intersection
// 10. fromCenterHalfExtents produces correct min/max

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "physics/AABB.hpp"

// ── Minimal test infrastructure ───────────────────────────────────────────────

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const char* testName) {
    if (condition) {
        std::printf("[PASS] %s\n", testName);
        ++g_passed;
    } else {
        std::printf("[FAIL] %s\n", testName);
        ++g_failed;
    }
}

static bool approxEq(float a, float b, float eps = 1e-6f) {
    return std::fabs(a - b) < eps;
}

// ── Tests ─────────────────────────────────────────────────────────────────────

int main() {
    std::printf("Running AABB correctness tests...\n\n");

    // ── Test 1: Clearly separated boxes -- should NOT intersect ──────────────
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        AABB b(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(6.0f, 1.0f, 1.0f));
        check(!a.intersects(b), "Test 1: Two clearly separated boxes -- no intersection");
        check(!b.intersects(a), "Test 1b: Symmetric -- separated (B vs A)");
    }

    // ── Test 2: Partial overlap on all axes -- should intersect ──────────────
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f));
        AABB b(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
        check( a.intersects(b), "Test 2: Partial overlap on all axes -- intersection");
        check( b.intersects(a), "Test 2b: Symmetric -- partial overlap (B vs A)");
    }

    // ── Test 3: One box inside another -- should intersect ───────────────────
    {
        AABB outer(glm::vec3(-5.0f, -5.0f, -5.0f), glm::vec3(5.0f, 5.0f, 5.0f));
        AABB inner(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        check( outer.intersects(inner), "Test 3: Inner box inside outer -- intersection");
        check( inner.intersects(outer), "Test 3b: Symmetric -- inner vs outer");
    }

    // ── Test 4: Touching faces -- should intersect ───────────────────────────
    // Box A max.x == Box B min.x (shared face on X axis).
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        AABB b(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.0f));
        check( a.intersects(b), "Test 4: Touching faces (A.max.x == B.min.x) -- intersection");
        check( b.intersects(a), "Test 4b: Symmetric -- face touch (B vs A)");
    }

    // ── Test 5: Touching edges -- should intersect ───────────────────────────
    // A max.x == B min.x AND A max.y == B min.y (shared edge).
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 2.0f));
        AABB b(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f));
        check( a.intersects(b), "Test 5: Touching edges -- intersection");
        check( b.intersects(a), "Test 5b: Symmetric -- edge touch (B vs A)");
    }

    // ── Test 6: Touching corner -- should intersect ──────────────────────────
    // Only one corner point is shared.
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        AABB b(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
        check( a.intersects(b), "Test 6: Touching corner -- intersection");
        check( b.intersects(a), "Test 6b: Symmetric -- corner touch (B vs A)");
    }

    // ── Test 7: Separation on X only -- should NOT intersect ─────────────────
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        AABB b(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(3.0f, 1.0f, 1.0f));
        check(!a.intersects(b), "Test 7: Separated on X only -- no intersection");
    }

    // ── Test 8: Separation on Y only -- should NOT intersect ─────────────────
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        AABB b(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        check(!a.intersects(b), "Test 8: Separated on Y only -- no intersection");
    }

    // ── Test 9: Separation on Z only -- should NOT intersect ─────────────────
    {
        AABB a(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        AABB b(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 3.0f));
        check(!a.intersects(b), "Test 9: Separated on Z only -- no intersection");
    }

    // ── Test 10: fromCenterHalfExtents produces correct min/max ──────────────
    {
        glm::vec3 center(3.0f, 1.5f, -2.0f);
        glm::vec3 half(0.5f, 0.5f, 0.5f);
        AABB box = AABB::fromCenterHalfExtents(center, half);

        bool minOk =
            approxEq(box.min().x, 2.5f) &&
            approxEq(box.min().y, 1.0f) &&
            approxEq(box.min().z, -2.5f);

        bool maxOk =
            approxEq(box.max().x, 3.5f) &&
            approxEq(box.max().y, 2.0f) &&
            approxEq(box.max().z, -1.5f);

        check(minOk, "Test 10a: fromCenterHalfExtents -- correct min");
        check(maxOk, "Test 10b: fromCenterHalfExtents -- correct max");
    }

    // ── Summary ───────────────────────────────────────────────────────────────
    std::printf("\nResults: %d passed, %d failed.\n", g_passed, g_failed);

    if (g_failed > 0) {
        std::printf("AABB TESTS FAILED\n");
        return EXIT_FAILURE;
    }

    std::printf("All AABB tests passed.\n");
    return EXIT_SUCCESS;
}
