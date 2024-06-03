import java.io.IOException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    private static final List<String> products = List.of("ball", "book", "monitor", "board game");
    private static final int capacity = 100;
    public static void main(String[] args) {
        Warehouse warehouse = new Warehouse(capacity, products);

        int numberOfProducers = 3;
        int numberOfConsumers = 3;

        ExecutorService producers = Executors.newFixedThreadPool(numberOfProducers);
        ExecutorService consumers = Executors.newFixedThreadPool(numberOfConsumers);

        for (int i=0; i<numberOfProducers; i++){
            producers.submit(new Producer(warehouse, i));
        }
        for (int i=0; i<numberOfConsumers; i++){
            consumers.submit(new Consumer(warehouse, i));
        }

        try{
            if(System.in.read() == 10) { //10-enter
                //zamkniecie watkow
                warehouse.changeRunningState();
                producers.shutdown();
                consumers.shutdown();
                System.exit(0);
            }
        }catch(IOException e){
            e.printStackTrace();
        }
    }
}
