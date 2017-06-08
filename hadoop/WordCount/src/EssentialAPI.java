//
//import java.io.IOException;
//import java.net.URI;
//import java.net.URISyntaxException;
//
//import org.apache.commons.lang.StringUtils;
//import org.apache.hadoop.conf.Configuration;
//import org.apache.hadoop.fs.FileStatus;
//import org.apache.hadoop.fs.FileSystem;
//import org.apache.hadoop.fs.FileUtil;
//import org.apache.hadoop.fs.Path;
//import org.apache.hadoop.fs.PathFilter;
//import org.apache.hadoop.hdfs.DistributedFileSystem;
//import org.apache.hadoop.hdfs.protocol.DatanodeInfo;
//
//public class EssentialAPI {
//
//	private static final String HDFSUri = "/user/hadoop";
//
//	public static void main(String[] args) {
//		rmdir("/input/test");
//	}
//
//	/**
//	 * 文件上传至 HDFS
//	 * 
//	 * @param delSrc
//	 * @param overwrite
//	 * @param srcFile
//	 * @param destPath
//	 */
//	public static void copyFileToHDFS(boolean delSrc, boolean overwrite, String srcFile, String destPath) {
//		Path srcPath = new Path(srcFile);
//		String hdfsUri = HDFSUri;
//		if (StringUtils.isNotBlank(hdfsUri)) {
//			destPath = hdfsUri + destPath;
//		}
//		Path dstPath = new Path(destPath);
//		try {
//			FileSystem fs = getFileSystem();
//			fs.copyFromLocalFile(srcPath, dstPath);
//			fs.copyFromLocalFile(delSrc, overwrite, srcPath, dstPath);
//			fs.close();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//	}
//
//	/**
//	 * 删除文件或者文件目录
//	 * 
//	 * @param path
//	 */
//	public static void rmdir(String path) {
//		try {
//			// 返回FileSystem对象
//			FileSystem fs = getFileSystem();
//
//			String hdfsUri = HDFSUri;
//			if (StringUtils.isNotBlank(hdfsUri)) {
//				path = hdfsUri + path;
//			}
//
//			// 删除文件或者文件目录 delete(Path f) 此方法已经弃用
//			fs.delete(new Path(path), true);
//
//			// 释放资源
//			fs.close();
//		} catch (IllegalArgumentException | IOException e) {
//			e.printStackTrace();
//		}
//	}
//
//	/**
//	 * 创建文件目录
//	 * 
//	 * @param path
//	 */
//	public static void mkdir(String path) {
//		try {
//			// 获取文件系统
//			FileSystem fs = getFileSystem();
//			String hdfsUri = HDFSUri;
//			if (StringUtils.isNotBlank(hdfsUri)) {
//				path = hdfsUri + path;
//			}
//			// 创建目录
//			fs.mkdirs(new Path(path));
//			// 释放资源
//			fs.close();
//		} catch (IllegalArgumentException | IOException e) {
//			e.printStackTrace();
//		}
//	}
//
//	/**
//	 * 根据filter获取目录下的文件
//	 * 
//	 * @param path
//	 * @param pathFilter
//	 * @return String[]
//	 */
//	public static String[] ListFile(String path, PathFilter pathFilter) {
//		String[] files = new String[0];
//		try {
//			// 返回FileSystem对象
//			FileSystem fs = getFileSystem();
//			String hdfsUri = HDFSUri;
//			if (StringUtils.isNotBlank(hdfsUri)) {
//				path = hdfsUri + path;
//			}
//			FileStatus[] status;
//			if (pathFilter != null) {
//				// 根据filter列出目录内容
//				status = fs.listStatus(new Path(path), pathFilter);
//			} else {
//				// 列出目录内容
//				status = fs.listStatus(new Path(path));
//			}
//			// 获取目录下的所有文件路径
//			Path[] listedPaths = FileUtil.stat2Paths(status);
//			// 转换String[]
//			if (listedPaths != null && listedPaths.length > 0) {
//				files = new String[listedPaths.length];
//				for (int i = 0; i < files.length; i++) {
//					files[i] = listedPaths[i].toString();
//				}
//			}
//			// 释放资源
//			fs.close();
//		} catch (IllegalArgumentException | IOException e) {
//			e.printStackTrace();
//		}
//		return files;
//	}
//
//	/**
//	 * 获取文件系统
//	 * 
//	 * @return FileSystem
//	 */
//	public static FileSystem getFileSystem() {
//		// 读取配置文件
//		Configuration conf = new Configuration();
//		// 文件系统
//		FileSystem fs = null;
//		String hdfsUri = HDFSUri;
//		if (StringUtils.isBlank(hdfsUri)) {
//			// 返回默认文件系统 如果在 Hadoop集群下运行，使用此种方法可直接获取默认文件系统
//			try {
//				fs = FileSystem.get(conf);
//			} catch (IOException e) {
//				e.printStackTrace();
//			}
//		} else {
//			// 返回指定的文件系统,如果在本地测试，需要使用此种方法获取文件系统
//			try {
//				URI uri = new URI(hdfsUri.trim());
//				fs = FileSystem.get(uri, conf);
//			} catch (URISyntaxException | IOException e) {
//				e.printStackTrace();
//			}
//		}
//		return fs;
//	}
//
//	/**
//	 * 从 HDFS 下载文件
//	 * 
//	 * @param srcFile
//	 * @param destPath
//	 */
//	public static void getFile(String srcFile, String destPath) {
//		String hdfsUri = HDFSUri;
//		if (StringUtils.isNotBlank(hdfsUri)) {
//			srcFile = hdfsUri + srcFile;
//		}
//		Path srcPath = new Path(srcFile);
//		Path dstPath = new Path(destPath);
//		try {
//			FileSystem fs = getFileSystem();
//			fs.copyToLocalFile(srcPath, dstPath);
//			fs.close();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//	}
//
//	/**
//	 * 获取 HDFS 集群节点信息
//	 * 
//	 * @return DatanodeInfo[]
//	 */
//	public static DatanodeInfo[] getHDFSNodes() {
//		// 获取所有节点
//		DatanodeInfo[] dataNodeStats = new DatanodeInfo[0];
//		try {
//			// 返回FileSystem对象
//			FileSystem fs = getFileSystem();
//			// 获取分布式文件系统
//			DistributedFileSystem hdfs = (DistributedFileSystem) fs;
//			dataNodeStats = hdfs.getDataNodeStats();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//		return dataNodeStats;
//	}
//
//	/**
//	 * 判断目录是否存在
//	 * 
//	 * @param srcPath
//	 * @param dstPath
//	 */
//	public boolean existDir(String filePath, boolean create) {
//		boolean flag = false;
//		if (StringUtils.isEmpty(filePath))
//			return flag;
//		try {
//			Path path = new Path(filePath);
//			// FileSystem对象
//			FileSystem fs = getFileSystem();
//			if (create && !fs.exists(path))
//				fs.mkdirs(path);
//			if (fs.isDirectory(path))
//				flag = true;
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		return flag;
//	}
//
//	/**
//	 * 文件重命名
//	 * 
//	 * @param srcPath
//	 * @param dstPath
//	 */
//	public boolean rename(String srcPath, String dstPath) {
//		boolean flag = false;
//		try {
//			// 返回FileSystem对象
//			FileSystem fs = getFileSystem();
//			String hdfsUri = HDFSUri;
//			if (StringUtils.isNotBlank(hdfsUri)) {
//				srcPath = hdfsUri + srcPath;
//				dstPath = hdfsUri + dstPath;
//			}
//			flag = fs.rename(new Path(srcPath), new Path(dstPath));
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//		return flag;
//	}
//}
