
import java.util.*; 

class Schdeules {
    public static void main(String[] args) {
        Timer timer = new Timer();
        TimerTask task = new TimerTask() {
            int count = 0;
            @Override
            public void run() {
                if(++count == 5) timer.cancel();
                System.out.println("Task Nigga Task");
            }
        };


        System.out.println("Schedule: ");
        timer.schedule(task, 2000, 1000);
                // Comment me what is the difference between .schedule and .scheduleAtFixedRate? 


        
    }
}