package edu.sdust.students.classesgender;

import java.io.IOException;
import java.net.URI;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;

/**
 * 统计班级及人数
 * 
 * @author hadoop
 */
public class CountClassesGender {
	// hdfs://localhost:9000/user/hadoop/output/students/classes/part-00000
	static final String PATH = "hdfs://localhost:9000/user/hadoop/";
	static final String INPUT_PATH = PATH + "input/students/stus_ids_id";
	static final String OUT_PATH = PATH + "output/students/classes_gender";

	public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
		private final static IntWritable one = new IntWritable(1);
		private Text word = new Text();

		public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter)
				throws IOException {
			String line = value.toString().trim();
			StringTokenizer tokenizer = new StringTokenizer(line);
			String sid = tokenizer.nextToken();
			String id = tokenizer.nextToken();
			if (sid.startsWith("2014") && id.length() == 18) {
				String classes = sid.substring(0, 10);
				if ((id.charAt(16) - '0') % 2 == 0) {
					classes += "0";
				} else {
					classes += "1";
				}
				word.set(classes);
				output.collect(word, one);
			}
		}
	}

	public static class Reduce extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable> {
		public void reduce(Text key, Iterator<IntWritable> values, OutputCollector<Text, IntWritable> output,
				Reporter reporter) throws IOException {
			int sum = 0;
			while (values.hasNext()) {
				sum += values.next().get();
			}
			output.collect(key, new IntWritable(sum));
		}
	}

	public static void main(String[] args) throws Exception {

		final Configuration configuration = new Configuration();
		final FileSystem fileSystem = FileSystem.get(new URI(INPUT_PATH), configuration);
		if (fileSystem.exists(new Path(OUT_PATH))) {
			fileSystem.delete(new Path(OUT_PATH), true);
		}

		JobConf conf = new JobConf(CountClassesGender.class);
		conf.setJobName("CountClasses");

		conf.setOutputKeyClass(Text.class);
		conf.setOutputValueClass(IntWritable.class);

		conf.setMapperClass(Map.class);
		conf.setCombinerClass(Reduce.class);
		conf.setReducerClass(Reduce.class);

		conf.setInputFormat(TextInputFormat.class);
		conf.setOutputFormat(TextOutputFormat.class);

		FileInputFormat.setInputPaths(conf, new Path(INPUT_PATH));
		FileOutputFormat.setOutputPath(conf, new Path(OUT_PATH));

		JobClient.runJob(conf);
	}
}