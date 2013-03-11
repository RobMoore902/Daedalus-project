<?php

require_once('config/mysql.config.php');
$mysql_link = $mysql_link;

class Query {
  
  public static function escapeString($array) {
    global $mysql_link;
    
    $escaped_array = array();
    foreach($array as $key=>$val) {
      $escapedKey = mysqli_real_escape_string($mysql_link, $key);
      $escapedVal = mysqli_real_escape_string($mysql_link, $val);
      $escaped_array[$escapedKey] = $escapedVal;
    }
    
    return $escaped_array;
  }

  //generic query
  public static function d_query($query) {
    global $mysql_link;
    $x = mysqli_query($mysql_link, $query) or die(mysqli_error($mysql_link));
    return $x;
  }
  
  //insert
  public static function d_queryInsert($table, $input_array=null) {
  
    if(is_array($input_array)) {
      $columns = '';
      $values = '';
      $i = 0;
      $count = count($input_array);
      
      $input_array = self::escapeString($input_array);
      
      foreach($input_array as $col=>$val) {
        $columns .= $col;
        $values .= "'$val'";

        if($count > 1 && $i < ($count-1)) {
          $columns .= ", ";
          $values .= ", ";
        }
        
        $i++;
      }
      
      $query = "
        INSERT INTO `$table` ($columns)
        VALUES ($values)";
      
      self::d_query($query);
      $id = mysql_insert_id();
      
      if($table != 'ubicom_AoE') {
        $obj = self::d_queryRow($table, array('id'=>$id));
      }
      else {
        $obj = self::d_queryRow($table, array('uid'=>$id));
      }
      // print_r($obj);
      return $obj;
    }
    else {
      die('Error: insert query');
      
      return null;
    }
  }
  
  //deletes
  public static function d_queryDelete($table, $whereConditions) {
    
    $query = "
      DELETE FROM `$table` ";
    
    if(is_array($whereConditions)) {
      $query .= "WHERE ";
      
      $count = count($whereConditions);
      $i = 0;
      
      $whereConditions = self::escapeString($whereConditions);
      foreach($whereConditions as $col=>$val) {
        $query .= "$col = '$val' ";
        if($count > 1 && $i < ($count-1)) {
          $query .= "AND ";
        }
        $i++;
      }
      
      self::d_query($query);
    }
    else if($whereConditions) {
      die('Error: query where conditions');
      return null;
    }
    
    return;
  }
  
  //returns single row
  public static function d_queryRow($table, $whereConditions=null, $sort=null) {
    global $mysql_link;
    
    $query = "
      SELECT *
      FROM `$table` ";
    
    if(is_array($whereConditions)) {
      $query .= "WHERE ";
      
      $count = count($whereConditions);
      $i = 0;
      
      $whereConditions = self::escapeString($whereConditions);
      
      foreach($whereConditions as $col=>$val) {
        $query .= "$col = '$val' ";
        if($count > 1 && $i < ($count-1)) {
          $query .= "AND ";
        }
        $i++;
      }
      
      if($sort) {
        $query .= 'ORDER BY '.$sort;
      }
      
    }
    else if($whereConditions) {
      die('Error: query where conditions');
      return null;
    }
    
    $result = self::d_query($query);
    
    if($result->num_rows == 0) {
      return null;
    }
    $row = mysqli_fetch_assoc($result);
    
    // return $query;
    return (object)$row;
  }
  
  //returns single unique column
  public static function d_queryCol($table, $colName, $whereConditions=null, $sort=null, $limit=null) {
    global $mysql_link;
    
    $query = "
      SELECT DISTINCT(".$colName.")
      FROM `$table` ";
    
    if(is_array($whereConditions) && count($whereConditions) > 0) {
      $query .= "WHERE ";
      
      $count = count($whereConditions);
      $i = 0;
      
      $whereConditions = self::escapeString($whereConditions);
      
      foreach($whereConditions as $col=>$val) {
        $query .= "$col = '$val' ";
        if($count > 1 && $i < ($count-1)) {
          $query .= "AND ";
        }
        $i++;
      }
    }
    else if($whereConditions) {
      die('Error: query where conditions');
      return null;
    }
		
		if($sort) {
			$query .= 'ORDER BY '.$sort.' ';
		}
		
		if($limit) {
			$query .= 'LIMIT '.$limit.' ';
		}
		
    $result = self::d_query($query);
    
		if($result->num_rows == 0) {
      return null;
    }
		
    $row_array = array();
    while($row = mysqli_fetch_assoc($result)) {
      $row_array[] = $row[$colName];
    }
    
    return $row_array;
  }
  
  //returns all rows
  public static function d_queryAll($table, $whereConditions=null, $sort=null, $limit=null) {
    $query = "
      SELECT *
      FROM `$table` ";
    
    if(is_array($whereConditions)) {
      $query .= "WHERE ";
      
      $count = count($whereConditions);
      $i = 0;
      
      $whereConditions = self::escapeString($whereConditions);
      
      foreach($whereConditions as $col=>$val) {
        $query .= "$col = '$val' ";
        if($count > 1 && $i < ($count-1)) {
          $query .= "AND ";
        }
        $i++;
      }
    }
    else if($whereConditions) {
      die('Error: query where conditions');
      return null;
    }
		
		if($sort) {
			$query .= 'ORDER BY '.$sort.' ';
		}
		
		if($limit) {
			$query .= 'LIMIT '.$limit.' ';
		}
		
    $result = self::d_query($query);
    
		if($result->num_rows == 0) {
      return null;
    }
		
    $row_array = array();
    while($row = mysqli_fetch_assoc($result)) {
      $row_array[] = (object)$row;
    }
    
    return $row_array;
  }

}