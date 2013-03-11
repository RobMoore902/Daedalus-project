<?php
	require_once('./classes/ContentEngine.php');
	require_once('./classes/Courses.php');
	require_once('./classes/Core.php');
	require_once('./classes/Query.php');
	require_once('./classes/SLO.php');
  // $x = $course_obj = Query::d_queryRow('dae_course', array('course_code'=>'csci AND "course_number" = "2121"'));
  // if($x) {
    // Core::var_dump('asd');
  // }
  // else 
  // $x = Courses::getBrowseCourseList();
  // Core::var_dump($x);
	
	// $x = SLO::getSLOAlphaOrdering();
	// Core::var_dump($x);
	
	$generator = new ContentEngine();
?>

