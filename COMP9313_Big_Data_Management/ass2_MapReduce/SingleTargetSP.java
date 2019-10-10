package comp9313.ass2;

import java.io.*;
import java.net.URI;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;


public class SingleTargetSP {
	
    public static String OUT = "output";
    public static String IN = "input";

	//using for initialize the graph to become Adjacency List
	public static class AdjacencyList2Mapper extends Mapper<Object, Text, LongWritable, Text>{
		public void map(Object key, Text value, Context context) 
				throws IOException, InterruptedException{
			LongWritable node;
			Text distance;
			String[] list = value.toString().split(" ");
			if(list.length >= 4){
				node = new LongWritable(Long.parseLong(list[2]));
				distance = new Text(list[1].toString()+":"+list[3].toString() + "#" + list[1].toString());
				context.write(node, distance);
				
			}//end if

		}//end map
	}//end AdjacencyList2Mapper

	public static class AdjacencyList2Reducer extends Reducer<LongWritable, Text, LongWritable, Text>{	
		public void reduce(LongWritable key, Iterable<Text> values, Context context) throws IOException, InterruptedException{
			Text distance;
			LongWritable node;
			String t = context.getConfiguration().get("transport");
			node = key;
			String node_distance; 
			String k = key.toString();
			if(k.equals(t)){
				node_distance = new String(k + ":0.0" + "#" + k + ",");
			}
			else{
				node_distance = new String(k+":1.071509E301" + "#" + k + ",");
			}
			for(Text val:values){
				node_distance += (val.toString()+",");
			}//end for
			distance = new Text(node_distance);
			context.write(node, distance);
		}//end reduce
	}// end AdjacencyList2Reducer
	
	//This is the Mapper part which will be used to iteration
    public static class STMapper extends Mapper<Object, Text, LongWritable, Text> {

        @Override
       public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
            // YOUR JOB: map function
            // ... ...
			LongWritable node;
			Text distance;
			String[] pair = value.toString().split("\t");
			//list:[0:0.0#0, 4:2.0#4, 2:5.0#2], or [1:1.071509E301#1, 2:3.0#2, 0:10.0#0,]
			String[] list = pair[1].split(",");			
			String first_path = list[0].split("#")[1].toString();						//#			
			if(list[0].split("#")[0].split(":")[1].equals("1.071509E301")){		//list[0].split("#")[0]:0:0.0 or 1:1.071509E301		
				String Node_distance = ("NODE;"+pair[1].toString());
				distance = new Text(Node_distance);
				node = new LongWritable(Long.parseLong(list[0].split("#")[0].split(":")[0]));
				context.write(node, distance);
			}
			else{
				/*else the first element's distance is not 1.071509E301
				 * then,split all elements*/
				for(int i = 0; i<list.length; i++){
					String path2 = "";	//which is used to cooperate the nodeid to path //#
					//if i==0, it means the first element which doesn's need process
					if(i==0){
						String subnode1 = list[i].split(":")[0];
						String subdistance1 = ("VALUE;"+list[i].split(":")[1].toString());
						node = new LongWritable(Long.parseLong(subnode1));
						distance = new Text(subdistance1);
						context.write(node, distance);
					}
					else{
						String subnode = list[i].split(":")[0].toString();
						double sub_distance = Double.parseDouble(list[i].split("#")[0].split(":")[1].toString());
						double node_distance = Double.parseDouble(list[0].split("#")[0].split(":")[1].toString());
						path2 = (list[i].split("#")[1].toString()+"->"+first_path);
						String subdistance = ("VALUE;"+Double.toString(node_distance + sub_distance)+"#"+path2);
						node = new LongWritable(Long.parseLong(subnode));
						distance = new Text(subdistance);
						context.write(node, distance);
					}//end else
				}//end for
				String Node_distance2 = ("NODE;"+pair[1].toString());
				distance = new Text(Node_distance2);
				node = new LongWritable(Long.parseLong(list[0].split("#")[0].split(":")[0]));
				context.write(node, distance);
			}//end else
        }

    }
    //This is the Reducer part which will be used to iteration
    public static class STReducer extends Reducer<LongWritable, Text, LongWritable, Text> {

        @Override
        public void reduce(LongWritable key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
            // YOUR JOB: reduce function
            // ... ...
			LongWritable node;
			Text distance;

			double lowest = 1.071509E301;
			double dis;
			String store_other_node_and_distance = "";
			String path = "";
			Text check_item = new Text();
			
			for(Text val:values){
				String[] title_value = val.toString().split(";");
				/*assume key == 2, then title_value:[NODE, 2:1.071509E301#2,1:3.0#1,] 
				 * or [VALUE, 5.0#2,] */
 				if(title_value[0].equals("VALUE")){
 					String temp_path = title_value[1].split("#")[1];
					dis = Double.parseDouble(title_value[1].split("#")[0]);
					lowest = Math.min(lowest, dis);
					if(lowest == dis){
						path = temp_path;
					}
				}
 				//else if title_value:[NODE, 2:1.071509E301#2,1:3.0#1,]
				else if(title_value[0].equals("NODE")){	
					//the variance of check_item, which is used to the final check 
					check_item = new Text(title_value[0]+";" + title_value[1]); 
					//node_distance_list, which include nodes and distances
					String node_distance_list = title_value[1].split(",")[0];	
					
					//node_distance, which means the distance of the node
					String node_distance = node_distance_list.split("#")[0].split(":")[1];					
					String[] after_node = title_value[1].split(",");
					for(int i = 1; i < after_node.length; i++){
						store_other_node_and_distance += (after_node[i]+",");
					}
					dis = Double.parseDouble(node_distance);
					String temp_path2 = node_distance_list.split("#")[1];
					lowest = Math.min(lowest, dis);
					if(dis == lowest){
						path = temp_path2;
					}
					
				}
			}//end for
			node = key;
			distance = new Text(node + ":" + lowest + "#" + path.toString() + "," + store_other_node_and_distance);			
			context.write(node, distance);
			//add check formula
			Text check_distance = new Text("NODE;"+distance);
			String[] title_value2 = check_item.toString().split(";");
			if(title_value2[0].equals("NODE")){
				if(check_distance.equals(check_item)){

				}
				else{
					//context.getCounter(check_distance.toString(), check_item.toString()).increment(2);
					context.getCounter("check_distance", "check_item").increment(1);
				}								
			}//end if     	
        }//end reduce
    }

    //This part of mapreduce is used to format the final result to be the formal result which we wanted.
	public static class FinalStepMapper extends Mapper<Object, Text, LongWritable, Text>{		
		public void map(Object key, Text value, Context context) 
				throws IOException, InterruptedException{
			LongWritable node;
			Text distance_path;
			String[] l = value.toString().split("\t");
			String[] list = l[1].split(",");
			String[] target = list[0].split(":");
			String check = target[1].split("#")[0];
			//this part is used to select the exactly path which connect with the target node
			if(check.equals("1.071509E301")){
				
			}
			else{
				node = new LongWritable(Long.parseLong(target[0]));
				distance_path = new Text(target[1]);
				context.write(node, distance_path);
			}			
		}//end map
	}//end FinalStepMapper
	
	public static class FinalStepReducer extends Reducer<LongWritable, Text, LongWritable, Text>{		
		public void reduce(LongWritable key, Iterable<Text> values, Context context) 
				throws IOException, InterruptedException{
			LongWritable node;
			Text distance_path;
			
			node = key;
			for(Text val: values){
				String[] list = val.toString().split("#");
				String normalize_dis_path = (list[0] + "\t" + list[1]);
				distance_path = new Text(normalize_dis_path);
				context.write(node, distance_path);
			}
		}//end reduce
	}//end FinalStepReducer

    public static void main(String[] args) throws Exception {        

        IN = args[0];

        OUT = args[1];

        int iteration = 0;

        String input = IN;

        String output = OUT + iteration;
        
        //now_counter is used to count the Counter's number
        long now_counter = 0;

	    // YOUR JOB: Convert the input file to the desired format for iteration, i.e., 
        //create the adjacency list and initialize the distances
        Configuration conf1 = new Configuration();
        conf1.set("transport", args[2]);
		Job job1 = Job.getInstance(conf1, "AdjacencyList");
		FileSystem fs = FileSystem.get(conf1);
		
		job1.setJarByClass(SingleTargetSP.class);
		job1.setMapperClass(AdjacencyList2Mapper.class);
		job1.setMapOutputKeyClass(LongWritable.class);
	    job1.setMapOutputValueClass(Text.class);
	    job1.setReducerClass(AdjacencyList2Reducer.class);
	    job1.setOutputKeyClass(LongWritable.class);
	    job1.setOutputValueClass(Text.class);
		FileInputFormat.addInputPath(job1, new Path(input));
	    FileOutputFormat.setOutputPath(job1, new Path(output));
	    //System.exit(job1.waitForCompletion(true) ? 0 : 1);
	    job1.waitForCompletion(true);
	    input = output;
        iteration += 1;
        output = OUT + iteration;
        //entry the iteration part, and use the output to be the next iteration's input
	    boolean isdone = false;
	    Configuration conf2 = new Configuration();
	    FileSystem fs2 = FileSystem.get(new URI(OUT), conf2);
	    while (isdone == false) {
        	Job job2 = Job.getInstance(conf2, "SingleTargetSP");
    		job2.setJarByClass(SingleTargetSP.class);
        	job2.setMapperClass(STMapper.class);
        	job2.setMapOutputKeyClass(LongWritable.class);
        	job2.setMapOutputValueClass(Text.class);
    	    job2.setReducerClass(STReducer.class);
    	    job2.setOutputKeyClass(LongWritable.class);
    	    job2.setOutputValueClass(Text.class);
            FileInputFormat.addInputPath(job2, new Path(input));
            FileOutputFormat.setOutputPath(job2, new Path(output));
            job2.waitForCompletion(true);
            //delete the output folder in the previous iteration to save disk space
            Path temp_path = new Path(input);
            if(fs2.exists(temp_path)){
            	fs2.delete(temp_path, true);
            }
    	    if(iteration > 0){
    	    	int i = iteration - 1;
    	    	fs2.delete(new Path(OUT + i));
    	    }              
            
            input = output;           

            iteration ++;

            output = OUT + iteration;

            long value = job2.getCounters().findCounter("check_distance", "check_item").getValue();
            now_counter = value;

            //this is used to track the Counter's operating, if Counter doesn't count, then the iteration is stopped
            if(now_counter == 0 ){//now_counter == pre_counter
                isdone = true;
            }
        }//end iteration
        
        /*Extract the final result using another MapReduce job with only 1 reducer, 
         * and store the results in HDFS*/
	    Configuration conf3 = new Configuration();
	    FileSystem fs3 = FileSystem.get(new URI(OUT), conf3);
	    Job job3 = Job.getInstance(conf3, "Finalstep");					
	    job3.setJarByClass(SingleTargetSP.class);
		job3.setMapperClass(FinalStepMapper.class);		
	    job3.setMapOutputKeyClass(LongWritable.class);
	    job3.setMapOutputValueClass(Text.class);
	    job3.setReducerClass(FinalStepReducer.class);
	    job3.setOutputKeyClass(LongWritable.class);
	    job3.setOutputValueClass(Text.class);
	    job3.setNumReduceTasks(1);
	    
	    FileInputFormat.addInputPath(job3, new Path(input));
	    FileOutputFormat.setOutputPath(job3, new Path(args[1]));
	    job3.waitForCompletion(true);
        Path temp_path2 = new Path(input);
        //delete the output folder in the previous step to save disk space
        if(fs3.exists(temp_path2)){
        	fs3.delete(temp_path2, true);
        }
	    if(iteration > 0){
	    	int i = iteration - 1;
	    	fs3.delete(new Path(OUT + i));
	    }
	    System.exit(true ? 0 : 1);
    }
}
