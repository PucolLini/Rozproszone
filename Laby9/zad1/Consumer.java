import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

public class Consumer implements Runnable {
        private final Warehouse warehouse;
        private final List<String> productTypes;
        private final int id;
        private ThreadLocalRandom random;
        public Consumer(Warehouse warehouse, int id){
            this.id = id;
            this.warehouse = warehouse;
            this.productTypes = warehouse.getProductTypes();
            this.random = ThreadLocalRandom.current();
        }

        @Override
        public void run() {
            this.random = ThreadLocalRandom.current();

            while(warehouse.runningState()){
                //losowanie produktow i ich ilosci
                String product = productTypes.get(random.nextInt(productTypes.size()));
                int productsQuantity = random.nextInt(10) + 1;

                //wybieranie produktow i sprawdzenie czy jest ich tyle w magazynie

                warehouse.lock();
                if(warehouse.canTake(product, productsQuantity)){
                    System.out.println("Consumer " + id + ": took " + productsQuantity + " of " + product);
                } else {
                    System.out.println("Consumer " + id + ": wasn't able to take " + productsQuantity + " of " + product);
                }
                warehouse.unlock();

                try {
                    Thread.sleep(random.nextInt(5000)+1000);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
            System.out.println("Consumer " + id + " stopped");
        }
}