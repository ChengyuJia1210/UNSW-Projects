package comp9313.ass1;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;


public class EdgeAvgLen2 {
	//define an IntPair class to keep two elements
	public static class IntPair implements Writable{
		//First is sum value, Second is count number
		private double first;
		private int second;

		//Constructor IntPair
		public IntPair(double first, int second){
			set(first, second);
		}
		
		public IntPair(){
			
		}
		
		public void set(double left, int right){
			first = left;
			second = right;
		}
		
		public double getFirst(){
			return first;
		}
		public int getSecond(){
			return second;
		}

		@Override
		public void readFields(DataInput in) throws IOException{
			first = in.readDouble();
			second = in.readInt();
		}
		@Override
		public void write(DataOutput out) throws IOException{
			out.writeDouble(first);
			out.writeInt(second);
		}
	}
	
	//Rewrite Mapper class
	public static class EdgeAvgLen2Mapper extends Mapper<Object, Text, IntWritable, IntPair>{
		/*define two hashmap type record_sum(which is used to store key and sum value)
		 * and record_count(which is used to store key and count number)*/
		private Map<Integer, Double> record_sum  = new HashMap<Integer, Double>();
		private Map<Integer, Integer>record_count = new HashMap<Integer, Integer>();
		private IntPair pair = new IntPair();

		protected void setup(Context context) throws IOException, InterruptedException{

		}
		
		public void map(Object key, Text value, Context context) 
				throws IOException, InterruptedException{
			String[] list = value.toString().split(" ");
			/*through if to judge the list's length, 
			 * if less than 4, that means there is no income */
			if(list.length >= 4){
				/*through if to judge the list's third element, 
				 * if the third element's income is 0, that means there is no income */
				if(Double.parseDouble(list[3]) != 0){//Integer.parseInt(list[1]) != Integer.parseInt(list[2]) & 
					/*convert list[2] to become Integer type id(which is the key), 
					* list[3] to become Double type val which will be summed*/
					Integer id = Integer.parseInt(list[2]);
					Double val = Double.parseDouble(list[3]);
					//according to key(id) to combine to same key values(distance)
					if(record_sum.containsKey(id)){
						record_sum.put(id, (record_sum.get(id) + val));
					}
					//else create a new pair
					else{
						record_sum.put(id, val);
					}
					//according to key(id) to count the income times of the key 
					if(record_count.containsKey(id)){
						record_count.put(id, (record_count.get(id) + 1));
					}
					//else create a new pair
					else{
						record_count.put(id, 1);
					}
				}
			}
 		}//end of map function
		
		//according to cleanup function to emit the in-mapping combining results
		protected void cleanup(Context context) 
				throws IOException, InterruptedException{
			for(Map.Entry<Integer, Double> entry: record_sum.entrySet()){
				Integer key_id = entry.getKey();
				Double sum = entry.getValue();
				Integer count = record_count.get(key_id);
				pair.set(sum, count);
				IntWritable k = new IntWritable(key_id);
				context.write(k, pair);
			}
		}//end of cleanup
	}//end of EdgeAvgLen2Mapper class
	
	//Rewrite the Reducer class
	public static class EdgeAvgLen2Reducer extends Reducer<IntWritable, IntPair, IntWritable, DoubleWritable>{
		private DoubleWritable average = new DoubleWritable();
		
		public void reduce(IntWritable key, Iterable<IntPair> values, Context context) 
				throws IOException, InterruptedException{
			double sum = 0.0;
			int count = 0;
			//According to iterate the values, to get the IntPair elements val
			for(IntPair val: values){
				sum += val.getFirst();
				count += val.getSecond();
			}
			//define a double type variable to store the average value
			double a = sum/(double)count;
			average = new DoubleWritable(a);
			//write (key, average value) into context, and then emit
			context.write(key, average);
		}// end of reduce function
	}// end of EdgeAvgLen2Reducer class
	
	public static void main(String[] args) throws Exception{
		Configuration conf = new Configuration();
		Job job = Job.getInstance(conf, "edge average length2");
		job.setJarByClass(EdgeAvgLen2.class);
		job.setMapperClass(EdgeAvgLen2Mapper.class);
	    job.setMapOutputKeyClass(IntWritable.class);
	    job.setMapOutputValueClass(IntPair.class);
	    job.setReducerClass(EdgeAvgLen2Reducer.class);
	    job.setOutputKeyClass(IntWritable.class);
	    job.setOutputValueClass(DoubleWritable.class);
	    FileInputFormat.addInputPath(job, new Path(args[0]));
	    FileOutputFormat.setOutputPath(job, new Path(args[1]));
	    System.exit(job.waitForCompletion(true) ? 0 : 1);
	}
}
