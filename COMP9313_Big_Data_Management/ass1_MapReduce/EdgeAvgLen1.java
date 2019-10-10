package comp9313.ass1;

import org.apache.hadoop.io.Text;
import java.io.IOException;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.IntWritable;
import java.io.DataInput;
import java.io.DataOutput;
import org.apache.hadoop.io.Writable;


public class EdgeAvgLen1 {
	//define an IntPair class to keep two elements
	public static class IntPair implements Writable{
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
	}// end of IntPair class
	
	//Rewrite Mapper class
	public static class EdgeAvgLen1Mapper extends 
		Mapper<Object, Text, IntWritable, IntPair> {
		private  IntPair pair = new IntPair();
		
		@Override
		public void map(Object key, Text value, Context context) 
				throws IOException, InterruptedException{
			String[] list = value.toString().split(" ");
			/*through if to judge the list's length, 
			 * if less than 4, that means there is no income */
			if (list.length >= 4){
				/*through if to judge the list's third element, 
				 * if the third element's income is 0, that means there is no income */
				if(Double.parseDouble(list[3]) != 0){	//Integer.parseInt(list[1]) != Integer.parseInt(list[2]) & 
					//define an IntWritale type variable to store key
					IntWritable k = new IntWritable();
					k.set(Integer.parseInt(list[2]));
					double l = Double.parseDouble(list[3]);
					pair.set(l, 1);
					context.write(k, pair);
				}
			}
		}// end of map function	
	}// end of EdgeAvgLen1Mapper class
	
	//Rewrite the Reducer class to construct a combiner
	public static class EdgeAvgLen1Combiner  extends Reducer<IntWritable, IntPair, IntWritable, IntPair>{
		private IntPair result = new IntPair();

		@Override
		public void reduce(IntWritable key, Iterable<IntPair> values, Context context)
			throws IOException, InterruptedException{
			double sum = 0.0;
			int count = 0;
			//According to iterate the values, to get the IntPair elements val
			for(IntPair val : values){
				sum += val.getFirst();
				count += val.getSecond();
			}
			result.set(sum, count);
			context.write(key, result);
		}// end of reduce function
	}// end of EdgeAvgLen1Combiner class
	
	//Rewrite the Reducer class
	public static class EdgeAvgLen1Reducer extends Reducer<IntWritable, IntPair, IntWritable, DoubleWritable> { //DoubleWritable
		private DoubleWritable average = new DoubleWritable();
		
		@Override
		public void reduce(IntWritable key, Iterable<IntPair>values, Context context) 
				throws IOException, InterruptedException{
			double sum = 0.0;
			int count = 0;
			//According to iterate the values, to get the IntPair elements val
			for(IntPair val : values)	{	
				sum += val.getFirst();
				count += val.getSecond();
			}
			//define a double type variable to store the average value
			double a = sum/(double)count;
			average = new DoubleWritable(a);
			//write (key, average value) into context, and then emit
			context.write(key, average);
		}//end of reduce function
	}// end of EdgeAvgLen1Reducer class
	
	public static void main(String[] args) throws Exception{
		Configuration conf = new Configuration();
		Job job = Job.getInstance(conf, "edge average length1");
		job.setJarByClass(EdgeAvgLen1.class);
		job.setMapperClass(EdgeAvgLen1Mapper.class);
	    job.setMapOutputKeyClass(IntWritable.class);
	    job.setMapOutputValueClass(IntPair.class);
		job.setCombinerClass(EdgeAvgLen1Combiner.class);
	    job.setReducerClass(EdgeAvgLen1Reducer.class);
	    job.setOutputKeyClass(IntWritable.class);
	    job.setOutputValueClass(DoubleWritable.class);
	    FileInputFormat.addInputPath(job, new Path(args[0]));
	    FileOutputFormat.setOutputPath(job, new Path(args[1]));
	    System.exit(job.waitForCompletion(true) ? 0 : 1);
	}
}
