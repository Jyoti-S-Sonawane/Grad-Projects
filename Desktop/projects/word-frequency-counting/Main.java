/*
Implement a program to count the frequency of words in a text file. The text file is partitioned into N segments. Each segment is processed by a separate thread that outputs the intermediate frequency count for its segment. The main process waits until all the threads complete; then it computes the consolidated word-frequency data based on the individual threads’ output.
*/

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.*;

class WordFrequencyCounter implements Callable<HashMap> {
    List<String> lines;

    public WordFrequencyCounter(List<String> l) {
        lines = l;
    }

    @Override
    public HashMap<String, Integer> call() throws Exception {
        if (lines.size() == 0) {
            return new HashMap<>();
        }

        String content = String.join(" ", lines);
        String[] words = content.toLowerCase().split("\\s+");
        HashMap<String, Integer> map = new HashMap<>();

        for (int i = 0; i < words.length; i++) {
            map.put(words[i], map.getOrDefault(words[i], 0) + 1);
        }

        return map;
    }
}

public class Main {
    public static void main(String[] args) throws Exception {
        Scanner input = new Scanner(System.in);

        // inputFilepath = ./input.txt
        System.out.println("Enter file path: ");
        String inputFilepath = input.nextLine();

        List<String> lines = Files.readAllLines(Paths.get(inputFilepath));

        System.out.println("Enter N (number of segments): ");
        int numberOfSegments = input.nextInt();

        List<List<String>> segments = new ArrayList<>();
        int startLineNumber = 0;
        int numberOfLinesForEachThread = (int) Math.ceil(lines.size() * 1.0 / numberOfSegments);
        for (int i = 0; i < numberOfSegments; i++) {
            int endLineNumber = startLineNumber + numberOfLinesForEachThread;
            endLineNumber = Math.min(endLineNumber, lines.size());
            List<String> currentSegmentLines = lines.subList(startLineNumber, endLineNumber);
            segments.add(currentSegmentLines);
            startLineNumber = endLineNumber;
        }

        FutureTask<HashMap<String, Integer>>[] tasks = new FutureTask[numberOfSegments];

        for (int i = 0; i < numberOfSegments; i++) {
            WordFrequencyCounter callable = new WordFrequencyCounter(segments.get(i));
            FutureTask<HashMap<String, Integer>> task = new FutureTask(callable);
            Thread t = new Thread(task);
            t.start();
            tasks[i] = task;
        }

        Map<String, Integer> result = new HashMap<>();

        for (int i = 0; i < numberOfSegments; i++)
        {
            HashMap<String, Integer> subResult = tasks[i].get();

            for (Map.Entry<String, Integer> stringCount: subResult.entrySet()) {
                result.put(stringCount.getKey(),
                        result.getOrDefault(stringCount.getKey(), 0) + stringCount.getValue());
            }
        }

        String outputFilePath = "./output.txt";
        File outputFile = new File(outputFilePath);

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(outputFile))) {
            for (Map.Entry<String, Integer> entry : result.entrySet()) {
                writer.write(entry.getKey() + " : " + entry.getValue());
                writer.newLine();
            }
            System.out.println("Output is redirected to output.txt\n");
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}