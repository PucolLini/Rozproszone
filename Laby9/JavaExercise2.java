import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.*;

public class JavaExercise2 {

    private static class CachingPrimeChecker {
        // bezpieczne cachowanie wyników
        private final Map<Long, Boolean> cache = new ConcurrentHashMap<>();

        public boolean isPrime(final long x) {
            //czy wynik w cache
            Boolean cachedResult = cache.get(x);
            if (cachedResult != null) {
                return cachedResult;
            }

            boolean result = computeIfIsPrime(x);
            cache.put(x, result);
            return result;
        }

        private boolean computeIfIsPrime(long x) {
            final String currentThreadName = Thread.currentThread().getName();
            System.out.printf("\t[%s] Running computation for: %d%n", currentThreadName, x);
            try {
                // simulating long computations
                Thread.sleep(TimeUnit.SECONDS.toMillis(10));
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            if (x < 2) {
                return false;
            }
            for (long i = 2; i * i <= x; i++) {
                if (x % i == 0) {
                    return false;
                }
            }
            return true;
        }
    }

    public static void main(String[] args) {
        CachingPrimeChecker primeChecker = new CachingPrimeChecker();

        final int numberOfThreads = 4;
        ExecutorService executor = Executors.newFixedThreadPool(numberOfThreads);

        Scanner scanner = new Scanner(System.in);

        while (true) {
            List<Future<Boolean>> futures = new ArrayList<>();

            int numbersToCheck = 7;

            System.out.println("Enter " + numbersToCheck + " numbers:");
            long[] numbers = new long[numbersToCheck];
            for (int i = 0; i < numbersToCheck; i++) {
                numbers[i] = scanner.nextLong();
            }

            //czekanie na 4 watki aby wszystkie wystartowaly
            CountDownLatch latch = new CountDownLatch(numberOfThreads);

            for (long number : numbers) {
                futures.add(executor.submit(() -> {
                    latch.countDown();
                    boolean isPrime = primeChecker.isPrime(number);
                    System.out.printf("Number %d is prime: %b%n", number, isPrime);
                    latch.countDown();
                    return isPrime;
                }));
            }


            for (Future<Boolean> future : futures) {
                try {
                    // Oczekiwanie na zakończenie wszystkich zadań
                    future.get();
                } catch (InterruptedException | ExecutionException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
