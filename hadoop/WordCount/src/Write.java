import java.io.IOException;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.fs.*;

public class Write {
	public static void main(String[] args) throws IOException {
		Configuration conf = new Configuration();
		Path inFile = new Path("/user/hadoop/input/file3.txt");
		FileSystem hdfs = FileSystem.get(conf);
		FSDataOutputStream os = hdfs.create(inFile);
		os.writeUTF("Chinese Hadoop Community\n");
		os.flush();
		os.close();
	}
}