import java.util.HashMap;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantLock;

public class Warehouse {
    private final HashMap<String, Integer> inventory = new HashMap<>();
    private final int capacity;
    private final List<String> productTypes;
    private volatile AtomicBoolean isRunning;
    private final ReentrantLock lock;

    public Warehouse(int capacity, List<String> productTypes) {
        this.capacity = capacity;
        this.productTypes = productTypes;
        this.isRunning = new AtomicBoolean(true);
        this.lock = new ReentrantLock();

        for (String product : productTypes) {
            inventory.put(product, 0);
        }
    }
    public List<String> getProductTypes() {
        return productTypes;
    }
    public synchronized boolean canPut(String product, int amount){
        lock.lock();
        try {
            int currentAmount = inventory.getOrDefault(product, 0);
            if (currentAmount + amount > capacity) {
                return false;
            }
            inventory.put(product, currentAmount + amount);
            return true;
        } finally {
            lock.unlock();
        }
    }
    public synchronized boolean canTake(String product, int amount){
        lock.lock();
        try {
            int currentAmount = inventory.getOrDefault(product, 0);
            if (currentAmount < amount) {
                System.out.println("Warehouse: not enough resources (" + amount + " of " + product + ")");
                return false;
            }
            inventory.put(product, currentAmount - amount);
            return true;
        } finally {
            lock.unlock();
        }
    }
    public void lock(){
        lock.lock();
    }
    public void unlock(){
        try{
            lock.unlock();
        } catch (IllegalMonitorStateException e) {
            System.out.println("Cannot unlock unlocked lock!");
        }
    }
    public void changeRunningState(){
        isRunning.set(!isRunning.get());
    }
    public boolean runningState() {
        return isRunning.get();
    }
}
