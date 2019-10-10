package comp9313.ass3

// just calculate the average of the output value

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import org.apache.spark.rdd.RDD 


object Problem1 {
  def main(args: Array[String]) {
		val inputFile = args(0)
		val outputFolder = args(1)
		val conf = new SparkConf().setAppName("Problem1").setMaster("local")
		val sc = new SparkContext(conf)
		val input =  sc.textFile(inputFile)
		
		//select the each line which has more than 3 elements to avoid wrong data
		val word = input.filter(x => (x.split(" ").size>=4)).collect()
		val putin = sc.makeRDD(word)
		//select out the node and the distance
    val words = putin.map(x => (x.split(" ")(1),(x.split(" ")(3)).toDouble)).collect()
    val arrayWords = sc.parallelize(words)
    //use combineByKey function to construct (key, (distance_sum, frequency)) pair
    val resualt = arrayWords.combineByKey(
        (per_value)=>(per_value,1),
        (pair:(Double, Int), per_value)=>(pair._1 + per_value, pair._2 + 1),
        (pair1:(Double, Int), pair2:(Double, Int))=>(pair1._1 + pair2._1, pair1._2 + pair2._2)
		)
		//use distance_sum to divide frequncy to get the average distance
		val end = resualt.map{case(key, value)=>(key.toLong, value._1/value._2)}	
		//reordering the sesult, decsending average distance, and ascending nodeID
		val order = end.map(x=>(x._2, x._1)).groupByKey().sortBy(x=> x._1, false).map(x=>(x._1,x._2.toList. sortWith(_<_)))		
		val finalResult = order.flatMap{
      x =>
      val len = x._2.length
      val arr = new Array[(Long,Double)](len)
      for(i <- 0 until len){
        arr(i) = (x._2(i),x._1)
      }
      arr
    }
		val output = finalResult.map(x => x._1 + "\t" + x._2)
		output.saveAsTextFile(outputFolder)
  }
}
  