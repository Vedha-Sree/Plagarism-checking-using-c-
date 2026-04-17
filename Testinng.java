
import java.util.*;
import java.util.stream.Collectors;

public class Testinng {
    public static void main(String[] args) {
        Map<String,String> hm = new HashMap<>();
        hm.put("C","c");
        hm.put("B","b");
        hm.put("A","a");

        Comparator<String> valueComparator = new Comparator<String>(){
            public int compare(String k1, String k2){
                return hm.get(k1).compareTo(hm.get(k2));
            }
        };

        Map<String,String> sortedMap = new TreeMap<>(valueComparator);
        sortedMap.putAll(hm);
        System.out.println("Original Map: " + hm);
        System.out.println("Sorted Map by values: " + sortedMap);

        Collection<String> values = hm.values();

        List<String> valList = new ArrayList<>(values);
        Collections.sort(valList);  
        System.out.println("Values as List: " + valList);
        // System.out.println("Values as Set: " + valSet);
    }
}