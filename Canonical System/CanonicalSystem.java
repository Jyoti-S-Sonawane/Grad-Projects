/*
The coin changing problem has both a dynamic programming and a greedy solutions. The greedy solution will yield an optimal solution only if a canonical coin system is used. In a paper written by Xuan Cai (Google search "Xuan Cai canonical coin system"), a quadratic algorithm is given to determine if a coin system is canonical. The objective of this project is to experimentally validate the correctness of the quadratic algorithm.

You are to implement the dynamic programming and the greedy algorithms of the coin changing problem. You are also to implement the quadratic algorithm that determine whether a coin system is canonical. For a set of 4 random coins ranging from a value of 1 to 25, confirm whether the coin system is canonical using

1. The quadratic solution from Xuan Cai's paper
2. The solutions from the dynamic programming and the greedy algorithms. (If the coin system is canonical, the two solutions should match.)

Turn in the source code for all algorithms with the driver program running with at least 20 different random coin systems and the US coin system (values of {1, 5, 10, 25}).

Also, determine a 5th coin that can be added to the US coin system such that the greedy algorithm is still valid.
*/

import java.util.*;

public class CanonicalSystem {
    public static void main(String[] args) {
        part1();
        part2();
    }

    static void part1() {
        Integer[] coins;
        for (int i = 0; i < 20; i++) {
            if (i < 19) {
                coins = generateCoins();
            }
            else {
                coins = new Integer[] {1, 5, 10, 25};
            }

            System.out.println("Coins are " + Arrays.toString(coins));

            boolean isCanonicalTheorem6 = isCanonicalTheorem6(coins);

            // Theorem 3
            boolean countExampleFound = false;
            for (int x = coins[2] + 2; x < coins[2] + coins[3]; x++) {
                int greedyResult = coinChangeGreedy(coins, x);
                int dpResult = coinChangeDP(coins, x);
                if (greedyResult == dpResult) {
                    // System.out.printf("***** Results are equal, amount: %d, dp: %d, greedy: %d\n", x, dpResult, greedyResult);
                } else {
                    countExampleFound = true;
                    System.out.printf("Counter example found, amount: %d, dp: %d, greedy: %d\n", x, dpResult, greedyResult);
                    break;
                }
            }

            boolean isCanonicalUsingCounterExample = !countExampleFound;

            System.out.printf("isCanonical using Xuan Cai's algo: %b, isCanonical by checking counter example: %b\n", isCanonicalTheorem6, isCanonicalUsingCounterExample);

            if (isCanonicalUsingCounterExample != isCanonicalTheorem6) {
                System.out.println("Answers are not equal");
            }

            System.out.println("\n____________________________________________________________\n\n");
        }
    }

    static void part2() {
        Integer[] fiveCoinUSSystem = new Integer[] {1, 5, 10, 25, 50};

        boolean misMatchFound = false;
        for (int i = 10 + 2; i < 25 + 50; i++) {
            int greedyAns = coinChangeGreedy(fiveCoinUSSystem, i);
            int dpAns = coinChangeDP(fiveCoinUSSystem, i);
            System.out.printf("For five coin system %s, greedy answer is %d, dp answer is %d\n", Arrays.toString(fiveCoinUSSystem), greedyAns, dpAns);
            if (greedyAns != dpAns) {
                misMatchFound = true;
                break;
            }
        }

        if (misMatchFound) {
            System.out.printf("The given five coin system is not canonical");
        }
        else {
            System.out.printf("The given five coin system is canonical");
        }
    }

    static int coinChangeDP(Integer[] coins, int amount){
        int[] dp = new int[amount+1];
        for(int i=1; i<=amount; i++){
            int temp = -1;

            for(int c: coins){
                if(i-c<0 || dp[i-c]<0) continue;

                if(temp<0){
                    temp = dp[i-c]+1;
                }else{
                    temp = Math.min(temp, dp[i-c]+1);
                }
            }

            dp[i] = temp;
        }

        return dp[amount];
    }

    static int coinChangeGreedy(Integer[] coins,int amount)
    {
        int n = coins.length;
        List<Integer> listOfCoins = new ArrayList<>();
        for (int i = n - 1; i >= 0; i--)
        {
            while (amount >= coins[i])
            {
                amount -= coins[i];
                listOfCoins.add(coins[i]);
            }
        }

        if(amount > 0){
            return -1;
        }

        return listOfCoins.size();
    }

    public static Integer[] generateCoins(){
        Integer[] randomCoins;
        Set<Integer> coins = new HashSet<>();
        coins.add(1);
        for(;coins.size() < 4;){
            coins.add((int) (Math.random()*(25-1))+1);
        }

        randomCoins = coins.toArray(new Integer[0]);
        Arrays.sort(randomCoins);
        return randomCoins;
    }

    // Theorem 6
    static boolean isCanonicalTheorem6(Integer[] coins) {
        boolean areThreeNonCanonical = checkCanonical3(coins[0], coins[1], coins[2]) == false;
        double x = coins[3] * 1.0 / coins[2];
        int k = (x == (int)x) ? (int)x - 1 : (int)x;
        int greedyResult = coinChangeGreedy(coins, (k+1) * coins[2]);
        boolean condition2 = greedyResult > k + 1;
        //System.out.printf("condition1 result %b condition2 result %b\n", areThreeNonCanonical, condition2);

        if (areThreeNonCanonical || condition2) {
            return false;
        }

        return true;
    }

    // Theorem 4
    static boolean checkCanonical3(int c1, int c2, int c3) {
        for (int q = 1; c3 >= q * c2; q++) {
            int r = c3 - q * c2;
            if (r > 0 && r < (c2- q)) {
                return false;
            }
        }

        return true;
    }
}
