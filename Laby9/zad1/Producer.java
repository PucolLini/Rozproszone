import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

public class Producer implements Runnable {
    private final Warehouse warehouse;
    private final List<String> productTypes;
    private final int id;
    private ThreadLocalRandom random;
    public Producer(Warehouse warehouse, int id){
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

            //tworzenie produktow i sprawdzenie czy sie miesci w magazynie
            if(warehouse.canPut(product, productsQuantity)){
                System.out.println("Producer " + id + ": put " + productsQuantity + " of " + product);
            } else {
                System.out.println("Producer " + id + ": wasn't able to put " + productsQuantity + " of " + product);
            }

            try {
                //spanko
                Thread.sleep(random.nextInt(5000)+1000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
        System.out.println("Producer " + id + " stopped");
    }
}
