import math
import random
import time

def trial_division(n):
    """Deterministic naive approach. O(sqrt(n))"""
    if n <= 1:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    
    # Check odd numbers up to sqrt(n)
    for i in range(3, int(math.isqrt(n)) + 1, 2):
        if n % i == 0:
            return False
    return True

def fermat_test(n, k = 5):
    """Probabilistic Fermat Test. Higher k means higher accuracy."""
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0:
        return False
    
    for _ in range(k):
        a = random.randint(2, n - 2)
        # Check if a^(n-1) % n == 1
        if pow(a, n - 1, n) != 1:
            return False  # Definitely composite
        
    return True

def miller_rabin(n, k=5):
    """Probabilistic Miller-Rabin Test. Industry standard."""
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0:
        return False

    # Write n-1 as 2^r * d
    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2

    # Witness loop
    for _ in range(k):
        a = random.randint(2, n - 2)
        x = pow(a, d, n)
        
        if x == 1 or x == n - 1:
            continue
            
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False  # Definitely composite
            
    return True  # Probably prime

def run_experiment(numbers, k_rounds=5):
    print(f"{'Number':<15} | {'Actual':<8} | {'Trial Div (ms)':<16} | {'Fermat (ms)':<13} | {'Miller-Rabin (ms)':<18}")
    print("-" * 80)
    
    for n in numbers:
        # Benchmark Trial Division (Ground Truth for smaller numbers)
        start = time.perf_counter()
        res_trial = trial_division(n)
        time_trial = (time.perf_counter() - start) * 1000
        
        # Benchmark Fermat
        start = time.perf_counter()
        res_fermat = fermat_test(n, k=k_rounds)
        time_fermat = (time.perf_counter() - start) * 1000
        
        # Benchmark Miller-Rabin
        start = time.perf_counter()
        res_mr = miller_rabin(n, k=k_rounds)
        time_mr = (time.perf_counter() - start) * 1000
        
        # Format results (adding an asterisk if a probabilistic test failed accuracy)
        fermat_label = f"{'Prime' if res_fermat else 'Comp'}"
        if res_fermat != res_trial and n < 10**12: # tracking discrepancy against ground truth
            fermat_label += "*" 
            
        mr_label = f"{'Prime' if res_mr else 'Comp'}"
        if res_mr != res_trial and n < 10**12:
            mr_label += "*"

        actual_label = "Prime" if res_trial else "Comp"
        
        print(f"{n:<15} | {actual_label:<8} | {time_trial:<16.4f} | {time_fermat:<13.4f} | {time_mr:<18.4f}")

carmichaels = [561, 1105, 1729, 2465, 2821, 6601]
print("Testing Carmichael Numbers (Should all be False/Composite):")
for c in carmichaels:
    print(f"Num: {c} | Fermat: {fermat_test(c, k=3)} | Miller-Rabin: {miller_rabin(c, k=3)}")

def run_advanced_experiments():
    # --- EXPERIMENT A: The Carmichael Leak ---
    print("=== EXPERIMENT A: Accuracy on Carmichael Numbers ===")
    carmichaels = [561, 1105, 1729, 2465, 2821, 6601]
    
    # We run multiple trials because it's probabilistic
    trials = 1000
    fermat_failures = 0
    mr_failures = 0
    
    for c in carmichaels:
        for _ in range(trials):
            if fermat_test(c, k=2): # Low k to expose the flaw
                fermat_failures += 1
            if miller_rabin(c, k=2):
                mr_failures += 1
                
    print(f"Out of {len(carmichaels) * trials} tests on composite Carmichael numbers:")
    print(f"-> Fermat Test was fooled (said 'Prime'): {fermat_failures} times.")
    print(f"-> Miller-Rabin Test was fooled: {mr_failures} times.\n")


    # --- EXPERIMENT B: Scaling Boundaries ---
    print("=== EXPERIMENT B: Performance Scaling by Digit Size ===")
    # Primes of progressively larger digit sizes
    primes = [
        100003,                 # 6 digits
        10000019,               # 8 digits
        1000000007,             # 10 digits
        10000000019,            # 11 digits
        32416190071,            # 11 digits (larger)
    ]
    
    print(f"{'Digits':<8} | {'Trial Div (ms)':<16} | {'Miller-Rabin (ms)':<18}")
    print("-" * 50)
    for p in primes:
        digits = len(str(p))
        
        # Time Trial Division
        t0 = time.perf_counter()
        trial_division(p)
        t_trial = (time.perf_counter() - t0) * 1000
        
        # Time Miller-Rabin (k=10 for high security)
        t0 = time.perf_counter()
        miller_rabin(p, k=10)
        t_mr = (time.perf_counter() - t0) * 1000
        
        print(f"{digits:<8} | {t_trial:<16.4f} | {t_mr:<18.4f}")

if __name__ == "__main__":
    # Test suite: Small primes, composites, a Carmichael number (561), and medium numbers
    test_numbers = [
        101,          # Small Prime
        561,          # Carmichael Number (Fermat's nemesis)
        9973,         # Medium Prime
        10000,        # Even Composite
        84001,        # Composite
        100003,       # Prime
        1000000007,   # Large Prime
        1000000008,   # Large Composite
    ]
    
    print("--- Experiment 1: Standard Mixed Numbers (k=5) ---")
    run_experiment(test_numbers, k_rounds=5)
    print("\n" + "="*80 + "\n")
    
    # Experiment 2: Heavy stress test on a massive prime where Trial Division struggles
    print("--- Experiment 2: Massive Prime Stress Test ---")
    large_prime = [32416190071] # A known prime requiring ~180,000 loop iterations for trial division
    run_experiment(large_prime, k_rounds=5)
    run_advanced_experiments()