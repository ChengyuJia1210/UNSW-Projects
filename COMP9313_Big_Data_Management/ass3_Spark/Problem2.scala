package comp9313.ass3

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.graphx._
import org.apache.spark.SparkConf
import org.apache.spark.rdd.RDD 


object Problem2 {
  def main(args: Array[String]) {
		val inputFile = args(0)		
		val sourceNode = args(1)
		val conf = new SparkConf().setAppName("Problem2").setMaster("local")
		val sc = new SparkContext(conf)
		val input =  sc.textFile(inputFile)
		
		//filter length small than 3 elements
		val word = input.filter(x => (x.split(" ").size>=4))
		/*construct vertex(nodeID, distance),when nodeID equal to source ID the distance equal to 0, Infinity otherwise
		 * sourceID = 0, then(0, 0.0); (1, 1.071509E301)*/
		val vertex2 = word.map(y => (if(y.split(" ")(1) == sourceNode) (y.split(" ")(1).toLong, 0.0)
		  else (y.split(" ")(1).toLong, 1.071509E301))).collect()
		//consturct edge(sourceID, destinationID, distance)
		val edge2 = word.map(x => Edge(x.split(" ")(1).toLong, x.split(" ")(2).toLong, x.split(" ")(3).toDouble)).collect()
		val vertexRDD2: RDD[(Long, Double)] = sc.parallelize(vertex2)
		val edgeRDD2: RDD[Edge[Double]] = sc.parallelize(edge2)
		//initial Graph
		val graph2: Graph[Double, Double] = Graph(vertexRDD2, edgeRDD2)
		//use Pregel to Iterate searching the shortest distance, which can help us to find the linking nodes		
		val shortestPath = graph2.pregel(Double.PositiveInfinity)(
      (nodeID, Distance, newDistance) => math.min(Distance, newDistance), 
      triplet => {  
        if (triplet.srcAttr + triplet.attr < triplet.dstAttr) {
          Iterator((triplet.dstId, triplet.srcAttr + triplet.attr))
        } else {
          Iterator.empty
        }
      },
      (x,y) => math.min(x,y) 
    )
    //the linking nodes' distance would not be infinity, so if the distance not infinity count this node
    val countNum = shortestPath.vertices.map(x => if(x._2 != 1.071509E301) 1 else 0)
    countNum.collect().map(println)  
    val sumall = countNum.reduce((x, y) => x+y)
    System.out.println(sumall-1)  
  }
}