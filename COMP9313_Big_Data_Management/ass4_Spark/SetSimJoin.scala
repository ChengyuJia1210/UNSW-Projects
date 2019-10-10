package comp9313.ass4

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import org.apache.spark.rdd.RDD
import scala.collection.mutable.ArrayBuffer

object SetSimJoin {
   def main(args: Array[String]){
    val inputFile = args(0)
    val outputFile = args(1)
    val Threshold = args(2).toDouble
    val conf = new SparkConf().setAppName("Set Similarity Join").setMaster("local")
    val sc = new SparkContext(conf)
    val input = sc.textFile(inputFile)
    
    val split2_input = input.map(x => (x.split(" ")(0), x.split(" ").drop(1).toList.sorted))
    split2_input.collect().map(println)
    val PrefixLen_pair = split2_input.flatMap{
      x =>
      var len = x._2.length
      val prefixlen2 = ((len-((len.toDouble)*Threshold)).toInt + 1)
      val arr = new Array[(Int, String)](prefixlen2)
      val String_k_and_v = (x._1+":"+x._2.mkString(" "))
      for(i <- 0 until prefixlen2){
        arr(i) = (x._2(i).toInt, String_k_and_v)
    }
    arr
    }
    val group = PrefixLen_pair.groupByKey()
    
    val new_try = group.map{
      x=>
          val arr_store_result = new ArrayBuffer[(Int, (Int, Double))]
          for(k2 <- x._2){
            for(l2 <- x._2){
              if(k2 != l2){ 
                if((k2.split(":")(0)).toInt < (l2.split(":")(0)).toInt){
                  val k_value_list = k2.split(":")(1).split(" ")
                  val l_value_list = l2.split(":")(1).split(" ")
                  val unionrdd = (k_value_list union l_value_list).distinct  
                  val interrdd = k_value_list.intersect(l_value_list)
                  val similarity = interrdd.length.toDouble/unionrdd.length.toDouble
                  if(similarity >= Threshold){
                    val result = (k2.split(":")(0).toInt, (l2.split(":")(0).toInt, similarity))
                    arr_store_result += result
                  }                 
                }            
              }
            }           
          }
          arr_store_result
      }
        val red = new_try.flatMap(x=>x).distinct.groupByKey().map{
        w=>
          (w._1, w._2.toList.sortBy(v=>v._1))
      }
      val new_red = red.sortByKey().map{
        x=>
         x._2.map{
            y=>
              (x._1, y._1, y._2)
         }
      }       
      val fin = new_red.flatMap(x=>x).map{
        x=>
          ((x._1, x._2)+"\t"+x._3)
      }
      fin.saveAsTextFile(outputFile)
   }
}
