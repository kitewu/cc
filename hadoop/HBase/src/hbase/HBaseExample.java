package hbase;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.*;
import org.apache.hadoop.hbase.client.*;
import org.apache.hadoop.hbase.io.compress.Compression.Algorithm;
import org.apache.hadoop.hbase.util.*;

public class HBaseExample {
	static Configuration conf = HBaseConfiguration.create();

	// 通过HBaseAdmin HTableDescriptor来创建一个新表
	public static void create(String tableName, String columnFamily) throws Exception {
		Connection con = ConnectionFactory.createConnection(conf);
		Admin admin = con.getAdmin();
		TableName tn = TableName.valueOf(tableName);
		if (admin.tableExists(tn)) {
			System.out.println("Table exist");
			System.exit(0);
		} else {
			HTableDescriptor table = new HTableDescriptor(tn);
			table.addFamily(new HColumnDescriptor(columnFamily).setCompactionCompressionType(Algorithm.NONE));
			admin.createTable(table);
			System.out.println("Table create success");
		}
	}

	// 添加一条数据，通过HTable Put为已存在的表添加数据
	public static void put(String tableName, String row, String columnFamily, String column, String data)
			throws IOException {
		Connection con = ConnectionFactory.createConnection(conf);
		TableName tn = TableName.valueOf(tableName);
		Table table = con.getTable(tn);
		Put put = new Put(Bytes.toBytes(row));
		put.addColumn(Bytes.toBytes(columnFamily), Bytes.toBytes(column), Bytes.toBytes(data));
		table.put(put);
		System.out.println("put success");
	}

	// 获取tableName表里列为row的结果集
	public static void get(String tableName, String row) throws IOException {
		Connection con = ConnectionFactory.createConnection(conf);
		Table table = con.getTable(TableName.valueOf(tableName));
		Get get = new Get(Bytes.toBytes(row));
		Result result = table.get(get);
		System.out.println("get " + result);
	}

	// 通过HTable Scan来获取tableName表的所有数据信息
	public static void scan(String tableName) throws IOException {
		Connection con = ConnectionFactory.createConnection(conf);
		Table table = con.getTable(TableName.valueOf(tableName));
		Scan scan = new Scan();
		ResultScanner resultScanner = table.getScanner(scan);
		for (Result s : resultScanner) {
			System.out.println("Scan " + s);
		}
	}

	public static boolean delete(String tableName) throws Exception {
		Connection con = ConnectionFactory.createConnection(conf);
		Admin admin = con.getAdmin();
		TableName tn = TableName.valueOf(tableName);
		if (admin.tableExists(tn)) {
			try {
				admin.disableTable(tn);
				admin.deleteTable(tn);
			} catch (Exception e) {
				e.printStackTrace();
				return false;
			}
		}
		return true;
	}

	public static void main(String[] args) {
		String tableName = "hbase_test";
		String columnFamily = "c1";

		try {
			HBaseExample.create(tableName, columnFamily);
			HBaseExample.put(tableName, "row1", columnFamily, "column1", "data1");
			HBaseExample.get(tableName, "row1");
			HBaseExample.scan(tableName);
			// if(HBaseExample.delete(tableName)==true){
			// System.out.println("delete table "+ tableName+"success");
			// }
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}