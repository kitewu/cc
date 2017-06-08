package edu.sdust.students.addr;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.net.URI;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;

/**
 * 统计结果排序
 * 
 * @author hadoop
 *
 */
public class SortResult {
	static final String PATH = "hdfs://localhost:9000/user/hadoop/";
	static final String INPUT_PATH = PATH + "output/students/address/part-00000";
	static final String OUT_PATH = PATH + "output/students/address/result";

	public static void main(String[] args) throws Exception {
		final Configuration configuration = new Configuration();

		final FileSystem fileSystem = FileSystem.get(new URI(INPUT_PATH), configuration);
		if (fileSystem.exists(new Path(OUT_PATH))) {
			fileSystem.delete(new Path(OUT_PATH), true);
		}

		@SuppressWarnings("deprecation")
		final Job job = new Job(configuration, SortResult.class.getSimpleName());

		// 1.1 指定输入文件路径
		FileInputFormat.setInputPaths(job, INPUT_PATH);
		// 指定哪个类用来格式化输入文件
		job.setInputFormatClass(TextInputFormat.class);

		// 1.2指定自定义的Mapper类
		job.setMapperClass(MyMapper.class);
		// 指定输出<k2,v2>的类型
		job.setMapOutputKeyClass(NewK2.class);
		job.setMapOutputValueClass(IntWritable.class);

		// 1.3 指定分区类
		job.setPartitionerClass(HashPartitioner.class);
		job.setNumReduceTasks(1);

		// 1.4 排序、分区
		// 1.5 （可选）合并

		// 2.2 指定自定义的reduce类
		job.setReducerClass(MyReducer.class);
		// 指定输出<k3,v3>的类型
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(IntWritable.class);

		// 2.3 指定输出到哪里
		FileOutputFormat.setOutputPath(job, new Path(OUT_PATH));
		// 设定输出文件的格式化类
		job.setOutputFormatClass(TextOutputFormat.class);
		// 把代码提交给JobTracker执行
		job.waitForCompletion(true);
	}

	// Mapper类的实现
	static class MyMapper extends Mapper<LongWritable, Text, NewK2, IntWritable> {
		protected void map(LongWritable key, Text value, Context context)
				throws java.io.IOException, InterruptedException {

			StringTokenizer tokenizer = new StringTokenizer(value.toString());
			String mkey = tokenizer.nextToken().toString();
			Integer mvalue = Integer.valueOf(tokenizer.nextToken());
			final NewK2 k2 = new NewK2(mkey, mvalue);
			final IntWritable v2 = new IntWritable(mvalue);
			context.write(k2, v2);
		};
	}

	// Reducer类的实现
	static class MyReducer extends Reducer<NewK2, IntWritable, Text, IntWritable> {
		protected void reduce(NewK2 k2, Iterable<IntWritable> v2s, Context context)
				throws java.io.IOException, InterruptedException {
			context.write(new Text(k2.key), new IntWritable(k2.value));
		};
	}

	static class NewK2 implements WritableComparable<NewK2> {
		String key;
		Integer value;

		public NewK2() {
		}

		public NewK2(String key, Integer value) {
			super();
			this.key = key;
			this.value = value;
		}

		@Override
		public void readFields(DataInput in) throws IOException {
			this.key = in.readUTF();
			this.value = in.readInt();
		}

		@Override
		public void write(DataOutput out) throws IOException {
			out.writeUTF(key);
			out.writeInt(value);
		}

		@Override
		public int hashCode() {
			return this.hashCode();
		}

		@Override
		public int compareTo(NewK2 o) {
			int cha = o.value - this.value;
			if (cha != 0)
				return cha;
			return o.key.compareTo(this.key);
		}

		@Override
		public boolean equals(Object obj) {
			if (!(obj instanceof NewK2)) {
				return false;
			}
			NewK2 oK2 = (NewK2) obj;
			return (this.key.equals(oK2.key)) && (this.value == oK2.value);
		}
	}
}
