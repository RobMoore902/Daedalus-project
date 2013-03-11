<?php
require_once('Query.php');
require_once('Core.php');
require_once('Courses.php');
require_once('SLO.php');
require_once('Graphviz_builder.php');
require_once('homepage.php');

Class ContentEngine{
	
	protected $link; //mysql link
	protected $data; //the html data
	protected $path; //the page args split into a '/' delimited array
	protected $page; //path[0]

	//the constructor - manage stuff
	public function __construct(){
		
		//create the database connection
		require('./config/mysql.config.php');
		$this->link = $mysql_link;

		//initialize the data 
		$this->data = '';

		//build some url information
		$this->path = explode('/',(isset($_GET['q']))?$_GET['q']:'');
		$this->page = $this->path[0];

		//get all the content
		$this->generateContent();
		
		//and now print everything
		print $this->data;

	}	
	
	//Cleanup - close db connection
	public function __destruct(){
		mysqli_close($this->link);
	}

	//Currently this simply returns "Daedalus", 
	//but can easily modified to return additional 
	//information for other information
	private function getTitle(){
		return 'Daedalus';
	}

	//this is the main function - and 
	//is used to determine which function
	//is used to generate the content
	private function generateContent(){
		
		
		//get the first part for determining which function.
		//each page is generated in the form of selected_(leftWing|center|rightWing)
		//once you know what function is called, call it. if the function does not exist,
		//it calls the default function.
		$leftWing = $rightWing = $content = '';
		$selected = '';


		
		if( isset($this->path[2]) ){
			if( $this->path[0] == 'course' ){//q=course/csci/*1101*
				$selected = 'course';
			}
			else if ( $this->path[0] == 'slo' && $this->path[1] == 'cloud' ){
				$selected = 'tagCloud';
			}
			else{
				redirect_home();
			}
		}
		else if( isset($this->path[1]) ){
			if( $this->path[0] == 'course' ){
				$selected = 'department';
			}
			else if($this->path[0] == 'slo' ){
				if( is_numeric($this->path[1])) {
					$selected = 'slo';
				}
				else if( $this->path[1] === 'cloud' ){
					$selected = 'tagCloud';
				}
				else{
					redirect_home();
				}
			}
			else if($this->path[0] == 'search' ){
				$selected = 'searchResults';
			}
			else{
				redirect_home();
			}
		}
		else{
			switch( $this->path[0] ){
				case 'course':
						$selected = 'courses';
					break;
				case 'slo':
						$selected = 'slos';
					break;
				case 'search':
						$selected = 'search';
					break;
				default:
						$selected = 'default';
					break;
			}
		
		}
		
		$leftWing = $selected.'_leftWing';
		$content = $selected.'_content';
		$rightWing = $selected.'_rightWing';
		
		if( ! method_exists($this,$leftWing) ){	
			$leftWing = 'default_leftWing';
		}
		if( ! method_exists($this,$content) ){
			$content = 'default_content';
		}
		if( ! method_exists($this,$rightWing) ){
			$rightWing = 'default_rightWing';
		}
				
		//add the left wing
		$this->addLeftWing($leftWing);
		
		//add the main content
		$this->addContent($content);
		
		//add the right wing
		$this->addRightWing($rightWing);
		
		//wrap the twitter bootstrap container div
		//around all the generated content
		$this->addContainer();		
		
		//add the navigation bar
		$this->prependNavBar();
		
		//wraps the body tag around everything
		$this->addBody();
		
		//adds all the <head> information
		$this->prependHead();
		
		//wraps everything in the html tag
		$this->addHtml();
		
	}
	
	//returns the string for breadcrumbs
	private function getBreadcrumbs() {
		//don't put anything on the main page
		if( sizeof($this->path) == 0 || empty($this->path[0] )){
			return;
		}

		$args = $this->path;		
		
		$breadcrumb_string = '
			<ul class="breadcrumb">
				<li>
					<a href="index.php">Home</a> <span class="divider">/</span>
				</li>';
		
		$url = 'index.php?q=';
		
		if(count($this->path) == 0 || !$args[0]) {
			return $breadcrumb_string .= '
				</ul>';
		}
		$arrsize = count($args);
		for($i = 0; $i<$arrsize-1; $i++) {
			$breadCrumb_arg = strtolower($args[$i]);
			if($i == 0) {
				$url .= $breadCrumb_arg;
			}
			else {
				$url .= '/'.$breadCrumb_arg;
			}
			
			$breadcrumb_string .= '<li>
				<a href="'.$url.'">';
			
			if( $this->path[0] == 'course' && $i==1 ){
				$breadcrumb_string .= strtoupper($args[$i]);
			}
			else if( $this->path[0] == 'slo' && $i==1 ){
				$slo = SLO::getSLO($args[$i]);
				$breadcrumb_string.= $slo->slo_text;
			}else{
				$breadcrumb_string .= ucfirst(strtolower($args[$i]));
			}
			
			
			$breadcrumb_string.= '</a>';
			
			if( $i != $arrsize - 2 ){
				$breadcrumb_string .= '<span class="divider">/</span></li>';
			} 
				
				
		}
		$breadcrumb_string .= '</ul>';
		
		return $breadcrumb_string;
	}
	
	private function addContainer(){
		
		$this->data = '
		<div class="container-fluid">
			<div class="row-fluid">
        <div class="span2"></div>
        <div class="span8">
          '.$this->getBreadcrumbs().'
         </div>
			</div>	
			<div class="row-fluid">
				'.$this->data.'
			</div><!--close row fluid-->
		</div><!--close container fluid-->';
	
	}

	//add body
	private function addBody(){
		$this->data = "<body>\n$this->data\n\t\t</body>";
	}

	//wrap html around it
	private function addHtml(){
		$this->data = '<!DOCTYPE html>'."\n".'<html>'."\n" . $this->data ."\n". '</html>'."\n";
	}

	//include all the head information - including all the files which should be included
	private function prependHead(){
		$this->data = '
		<head>
			<title>'.$this->getTitle().'</title>
			<link href="css/bootstrap.css" rel="stylesheet">       		
			<link href="css/style.css" rel="stylesheet" type="text/css" /a>    
			<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.js"></script>
			<script type="text/javascript" src="js/bootstrap-tab.js"></script>
			<script type="text/javascript" src="js/courseContent.js"></script>
			<script type="text/javascript" src="js/bootstrap-tooltip.js"></script>
			<script type="text/javascript" src="js/bootstrap-popover.js"></script>
			<script type="text/javascript" src="js/bootstrap-dropdown.js"></script>
			<script>  
				$(function (){ 
					$(".right-wing-item").popover(
						{placement: \'left\'}
					);
			  		$(".left-wing-item").popover();
				});  
			</script>
			<style>       
				body{ 
					padding-top: 60px; 
				}
			</style>
		</head>
		'.$this->data;     
	}
	
	//prepend the navbar information
	private function prependNavBar(){
		$this->navBarDefault();
	}

	//the navigation bar
	//follows the structure defined 
	//on twitter bootstrap	
	private function navBarDefault(){
		$dropDownItems = "";							
		$code = Courses::getCourseCodeList();
		foreach($code as $value)
		{
			$li = "<li><a href=\"index.php?q=course/".$value."\">";
			$li .= $value;
			$li .= "</a></li>\n";
			
			$dropDownItems .= $li;
		}
					
		$this->data.= '
		<div class="navbar navbar-fixed-top">
			<div class="navbar-inner">
				<div class="container-fluid">
					<a class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
						<span class="icon-bar"></span>
						<span class="icon-bar"></span>
						<span class="icon-bar"></span>
					</a>
					<a class="'.($this->page==''?'brand-select':'brand').'" href="index.php">Daedalus</a>
					<div class="btn-group pull-right">
						<div class="input-prepend">
							<form class="navbar-search pull-left" action="index.php?q=search" method="POST">
								<button type="submit" class="btn"><i class="icon-search"></i></button>
								<input type="text" class="search-query" placeholder="Search" id="search" name="terms">
							</form>
						</div>
					</div>	
					<div class="nav-collapse">
						<ul class="nav">        
							<li class="dropdown '.($this->page=='course'?'active':'').'">
							<a href="#" class="dropdown-toggle" data-toggle="dropdown">
								Browse Courses
							</a>
								<ul class="dropdown-menu">
									<li><a href="index.php?q=course">All Courses</a></li>
									 <li class="divider"></li>
									'.
										$dropDownItems
									.'					
								</ul>
							</li>
						</ul>
						<ul class="nav">
							<li class="dropdown '.($this->page=='slo'?'active	':'').'">
								<a href="#" class="dropdown-toggle" data-toggle="dropdown">
									Browse Learning Outcomes
								</a>
								<ul class="dropdown-menu">
									<li><a href="index.php?q=slo">Learning Outcomes</a></li>
									<li><a href="index.php?q=slo/cloud">Tag Cloud</a></li>
								</ul>
							</li>
						</ul>    
					</div><!--/.nav-collapse -->      
				</div>      
		    </div>
		</div>';
	}
	
	//make the call for the leftWing
	private function addLeftWing($func){	
		//no matter what - there needs to be span2. do that, then call.
		$this->data .= '<div class="span2">';
		if( method_exists($this,$func) ) $this->$func();
		else $this->default_leftWing();
		$this->data.= '</div>';
	}	

	//add the main content
	private function addContent($func){

		$this->data .= '
		<div class="container span8">';
		
		//TODO: make reliable
		if( $this->path[0] == 'course' && isset($this->path[1]) && isset($this->path[2])  ){
			
			$valid_course = Courses::courseExists($this->path[1], $this->path[2]);
      
      		if( $valid_course === false ){

				header( 'Location: index.php');
				die('Course not found');
			}
      
			$course_obj = Query::d_queryRow('dae_course', array('course_code'=>strtoupper($this->path[1]), 'course_number'=>$this->path[2]));
			
			$link = graphviz($course_obj->course);
			
      		//content for graph goes into this div here
			$this->data.= '
				<div class="well">
					<div class="page-header">
						<h1>'.$course_obj->course .' - '. $course_obj->course_name .'</h1>
					</div>
					<table align="center">
						<tr>
							<td>
								<img src="'.$link.'?rand='.rand(1,1000).'">
							</td>
						</tr>
						</table>
				</div>
			';
		
		}
		
		
		
		$this->data.='<div class="well">';

		( method_exists($this,$func) ? $this->$func() : $this->default_content );
		
		$this->data.= '	
			</div>
		</div>';
	}

	private function addRightWing($func){
		
		$this->data .= '
		<div class="span2">'; //fun should have <div class="well sidebar-nav">'; to be visible
		
		if( method_exists($this,$func) ) $this->$func();
		else $this->default_rightWing();
			
		$this->data.= '</div><!--close span2-->';
	}
		
	//default left wing		
	private function default_leftWing(){
		/*
			Here is the structure for how a wing should be setup when there is content
			<div class="well sidebar-nav">
				<ul class="nav nav-list">
				  <li class="nav-header">Left Sidebar</li>
				  <li class="active"><a href="#">Link</a></li>
				  <li><a href="#">Link</a></li>
				  <li><a href="#">Link</a></li>
				  <li><a href="#">Link</a></li>
				  <li><a href="#">Link</a></li>
				  <li><a href="#">Link</a></li>
				  <li><a href="#">Link</a></li>	
				</ul>
			</div>';
        */
		return;
		

		
	}
	
	private function default_content(){
		$this->data .= homepage::add_homepage();
	}

	private function default_rightWing(){
		//see default_leftWing to see how to structure the wings.
		return;
	}		

	//search page - but when there is no search already made
	private function search_content(){

		if( isset($_POST['terms'] )){

			header('Location: index.php?q=search/'.$_POST['terms']);
		}
		
		/* <form method="POST" action="index.php" >
                        <input type="text" id="query-bar" name="terms" />
                        <input type="hidden" name="q" value="search"/>
                        <input type="submit"/>
                </form>
                        ';
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                          
        <form class="navbar-search pull-left" action="index.php?q=search" method="POST">
			<span class="add-on">	
				<i class="icon-search"></i>
			</span>
			<input type="text" class="search-query" placeholder="Search" id="search" name="terms">
		</form>                
        
        */
		
		$this->data.= '
		<form class="well form-search" action="index.php?q=search" method="POST">
			<input type="text" class="input-medium search-query" plalceholder="Search" id="query-search" name="terms" />
			<input type="submit" class="btn"/>
		</form>

			';	
		
	}
	
	//page for search results	
	private function searchResults_content(){


		if( empty($this->path[1])){
			header('Location: index.php?q=search&failed=true');
		}
		
		$terms = preg_split('/\s+/', $this->path[1]);
		
		$results = array();
		$course_info = array();
		$slo_info = array();
	

		$safe_input = implode('|',$terms);
		$regex = ' REGEXP "(^|[[:blank:]])[[:alnum:]]{0,5}('.$safe_input.')[[:alnum:]]{0,5}([[:blank:]]|$)" ';
		$results = array();

		
		//first get Courses
		$query = '
			SELECT 
				*
			FROM 
				`dae_course` INNER JOIN `dae_course_slo`
					on `dae_course`.`id` = `dae_course_slo`.`course_id`
				INNER JOIN `dae_slo`
					on `dae_slo`.`id` = `dae_course_slo`.`slo_id`
			WHERE
				`course_code` ' . $regex.' 
				OR `course_number` ' . $regex . ' 
				OR `course_name` ' . $regex . '
				OR `slo_text` ' . $regex . '
			GROUP BY `dae_course`.`id`
			ORDER BY `course_code`,`course_number` asc

		';
				
		$result = mysqli_query($this->link,$query) or die('failed to query courses');
		
		while($row = mysqli_fetch_assoc($result) ){
			$cid = $row['course_id'];
			$dat = htmlspecialchars("$row[course_code] $row[course_number] - $row[course_name]");
			$results[$cid] = array();
			$course_info[$cid] = $dat;
		}
								
		//Now get SLOs - with courses
		$query = '
		
			SELECT 
				*
			FROM 
				`dae_slo` INNER JOIN `dae_course_slo`
					on `dae_slo`.`id` = `dae_course_slo`.`slo_id`
			WHERE
				`slo_text` '.$regex .'
				AND `dae_course_slo`.`course_id` IS NOT NULL
			ORDER BY `dae_slo`.`slo_text`
		';
		
		$result = mysqli_query($this->link,$query) or die( 'failed to collect learning outcomes' );
		while($row = mysqli_fetch_assoc($result)){
	
			$results[$row['course_id']][] = $row['slo_id'];
			$slo_info[$row['slo_id']] = $row['slo_text'];
		}

		$query = '
		
			SELECT 
				*
			FROM 
				`dae_slo` INNER JOIN `dae_course_slo`
					on `dae_slo`.`id` = `dae_course_slo`.`slo_id`
			WHERE
				`slo_text` '.$regex .'
				AND `dae_course_slo`.`course_id` IS NOT NULL
			ORDER BY `dae_slo`.`slo_text`
		';
		$results[-1] = array();
		$course_info[-1] = 'Learning outcomes with no associated course';
		
		$result = mysqli_query($this->link,$query) or die( 'failed to collect learning outcomes' );
		while($row = mysqli_fetch_assoc($result)){
			if( ! in_array($row['slo_id'],$results[-1]) ){
				$results[-1][] = $row['slo_id'];
				$slo_info[$row['slo_id']] = $row['slo_text'];
			}
		}
		
		
		//Build output string
		$output = '';
		$aterms = str_replace(' ','|',$this->path[1]);
		$reg = '/(\w*('.$aterms.')\w*)/i';	
		$rereg = '<span class="keyword">$0</span>';
		
		foreach($results as $course_id=>$slos){
			if( ! empty($course_id) ){
				$output.= '<div class="search course-set">';
				$output.= '<div class="course-title">';
				
				$c = Courses::getCourse($course_id);
				$code = $c->course_code;
				$num = $c->course_number;
				
				if( is_numeric($course_id) && $course_id>0){
					$output.= '<a href="index.php?q=course/'.$code.'/'.$num.'">';
				}
				
				
				$output.= preg_replace($reg, $rereg,$course_info[$course_id]);
				
				if( is_numeric($course_id) && $course_id>0){
					$output.= '</a>';
				}
				$output.= '</div>';//course-title
				
				$output.= '<ul>';
				foreach($slos as $slo_id){
					$output.= '<div class="slo-title">
					<li><a href="index.php?q=slo/'.$slo_id.'">';
					$slo = SLO::getSLO($slo_id);
					$output.= preg_replace($reg,$rereg,$slo->slo_text);
					$output.= '</a></li>
					</div>';
				}
				$output.= '</ul>';
				$output.= '</div>';
			}
		}		
									
		$this->data.= $output;

		
	}

	//courses
	private function courses_leftWing(){
		$this->department_leftWing();
	}
	private function courses_content(){

		$courses_array = Courses::getBrowseCourseList();
		$num_cols = 2;		


		$string = '';
		foreach($courses_array as $courseCode=>$course_array) {
	
			$string.= '<table class="table table-condensed table-striped">';
			$string.= '<tr><th colspan="3">'.$courseCode.'</th></tr>';
			
			$course_array2 = array();
			foreach($course_array as $course_id=>$course){
				$course_array2[] = $course;
			}
			
			$first = 0;
			$half = ((int)(count($course_array2)/2));
			$mid = $half;
			
			for($i=0; $i<$half+1; $i++) {
				$string .= '<tr>';
				if($course_array2[$first]) {
					$string .= '<td class="span6">'.$course_array2[$first]->hyperlink .'</td>';
				}
				if($course_array2[$mid]) {
					$string .= '<td class="span6">'.$course_array2[$mid+1]->hyperlink .'</td>';
				}
				$string .= '</tr>';
				
				$first++;
				$mid++;
			}
			
			$string .='</table><br/><br/>';

		}
		
		$this->data .= $string;
	}

	//when a specific course is specified
	private function course_leftWing(){

		$string = '<ul class="nav nav-list well right-align">
			<li class="nav-header">prerequisie courses</li>';


		$args = $this->path;
		$course_obj = Query::d_queryRow('dae_course', array('course_code'=>strtoupper($args[1]), 'course_number'=>$args[2]), null);

		$preReq_array = Courses::getPrereq($course_obj->id);
    
		foreach($preReq_array as $obj) {
      		$string .= '<li><a href="index.php?q=course/csci/'.$obj->course_number .'"  class="left-wing-item" data-original-title="'.$obj->course_name.'">'. $obj->course .' <i class="icon-chevron-left"></i></a></li>';
	}
		$string.=
		'</ul>';
    
		$this->data .= $string;
  
	}
	private function course_content(){

		$args = $this->path;
		$course_obj = Query::d_queryRow('dae_course', array('course_code'=>strtoupper($args[1]), 'course_number'=>$args[2]), null);
		$slo_array = SLO::getCourseLOList($course_obj->id);

		$string = '
		<ul class="nav nav-tabs" id="courseTabs">
			<li class="active">
				<a href="#description">Description</a>
			</li>
			<li>
				<a href="#alo">Assumed Learning Outcomes</a>
			</li>
			<li>
				<a href="#slo">Student Learning Outcomes</a>
			</li>
		</ul>
		
		<div class="tab-content">
			<div class="tab-pane active" id="description">
				INSERT COURSE DESCRIPTION HERE WHEN INFO IS AVAILABLE
			</div>
			<div class="tab-pane" id="alo">'.
				'What the student enrolling in this course is assumed to be able to do. <br />'.
				$slo_array['alo'].
			'</div>
			<div class="tab-pane" id="slo">'.
				'Student learning outcomes that are covered by this course. <br />'.
				$slo_array['slo'].
			'</div>
		</div>';
		
		$this->data.= $string;//'view information for ' . $this->path[1] . ' ' . $this->path[2];
	}
	private function course_rightWing(){
		$args = $this->path;
		$course_obj = Query::d_queryRow('dae_course', array('course_code'=>strtoupper($args[1]), 'course_number'=>$args[2]), null);

		$preReq_array = Courses::getPostreq($course_obj->id);
    
		$string = '<ul class="nav nav-list well">
			<li class="nav-header">prerequisie for</li>';

		foreach($preReq_array as $obj) {
			$string .= '
				<li>
				<a href="index.php?q=course/'.strtolower($obj->course_code).'/'.$obj->course_number .'" class="right-wing-item" data-original-title="'.$obj->course_name.'"><i class="icon-chevron-right"></i>'. $obj->course .'</a></li>';
		}
    
		$this->data .= $string;
    	
	
	}
		
	//dept is selected
	private function department_leftWing(){
    
		$courseCode_array = Courses::getCourseCodeList();
    
		$string = '
		<ul class="well nav nav-list">
			<li class="nav-header">Departments</li>';
		foreach($courseCode_array as $code) {
			$string .= '<li><a href="index.php?q=course/'.strtolower($code).'">'.$code.'</a></li>';
		}
    
		$this->data .= $string;
	}
	private function department_content(){
	
		$args = $this->path;
		$courses_list = Courses::getBrowseCourseList();
    
		$output_string = '';
		foreach($courses_list[strtoupper($args[1])] as $obj) {
			$output_string .= '<i class="icon-tag"></i>'.$obj->hyperlink .'<br />';
		}
    
    
		$this->data .= $output_string;
	}
	
	//specific slo
	private function slo_content(){
		$args = $this->path;
		
		$postSLO_array = SLO::getSLOPostReq($args[1]);
		
		$slo_string = '';
		if(count($postSLO_array) > 0) {
			foreach($postSLO_array as $id=>$slo) {
				$slo_string .= '
					<i class="icon-chevron-right"></i>
					<a href="index.php?q=slo/'.$id .'">'. $slo .'</a>
					<br />';
			}
		}
		else {
			$slo_string = "Sorry, I would tell you if I knew. I don't have the SLO to explain";
		}
		
		$courses_array = Courses::getCoursesBySLO($args[1]);
		
		$course_string = '';
		if(count($courses_array) > 0) {
			foreach($courses_array as $obj) {
				$course_string .= '
					<i class="icon-chevron-right"></i>
					<a href="index.php?q=course/csci/'.$obj->course_number .'">'. $obj->course .' - '.$obj->course_name.'</a>
					<br />';
			}
		}
		else {
			$course_string = "There aren't any courses covering this SLO unfortunately.";
		}
		
		
		$slo_obj = SLO::getSLO($args[1]);
		$string = '
			<div class="page-header">
				<h1>'.$slo_obj->slo_text .'</h1>
			</div>
			<ul class="nav nav-tabs" id="sloTabs">
				<li class="active">
					<a href="#why">Why do I need to know that?</a>
				</li>
				<li>
					<a href="#courses">Courses Covering This Learning Outcome</a>
				</li>
			</ul>
			
			<div class="tab-content">
				<div class="tab-pane active" id="why">
					These are the student learning outcome(s) that directly require this student learning outcome.<br />'.$slo_string.'
				</div>
				<div class="tab-pane" id="courses">'.$course_string.'</div>
			</div>';
		
		$this->data.= $string;//'view information for ' . $this->path[1] . ' ' . $this->path[2];
	}

	//list the slos
	private function slos_content(){
	
		$args = $this->path;
		$course_obj = Query::d_queryRow('dae_course', array('course_code'=>strtoupper($args[1]), 'course_number'=>$args[2]), null);
		$slo_array = SLO::getCourseLOList($course_obj->id);
		$sortedSLO_array = SLO::getSLOAlphaOrdering();
		
		$tab_string = '
		<div class = "tabbable tabs-left">
			<ul class="nav nav-tabs" id="browseSLOTabs">
				<li class="active">
					<a href="#all">All</a>
				</li>';
		
		$alphabet = 'A';
		for($i=0; $i<26; $i++) {
			if(isset($sortedSLO_array[$alphabet])) {
				$tab_string .= '
				<li>
					<a href="#'.$alphabet.'">'.$alphabet.'</a>
				</li>';
				
			}
			$alphabet++;
			
		}
		$tab_string .= '</ul>';
		
		
		
		
		$alphabet = 'A';
		$tabContent_string = '
			<div class="tab-content">
				<div class="tab-pane active" id="all">
					'.$sortedSLO_array['all'].'
				</div>';
		for($i=0; $i<26; $i++) {
			$tabContent_string .= '
				<div class="tab-pane" id="'.$alphabet.'">
					'.$sortedSLO_array[$alphabet].'
				</div>';
			$alphabet++;
		}
		$tabContent_string .= '</div></div>';
		
		
		$this->data .= $tab_string.$tabContent_string;
	}
	
	//tag cloud page
	private function tagCloud_leftWing(){
		if( ! isset($this->path[2]) || empty($this->path[2])){	
			return;
		}

		$selected_tags = (empty($this->path[2]) ? false : explode('_',$this->path[2]));
		
		$output='<div class="well sidebar-nav">
			<ul class="nav nav-list">';
		
		$output .= '<li class="nav-header">'. ( $selected_tags === false ? '<i>No Selected tags</i>' : 'Selected tags' ). '</li>';
		
		//get the tags
		//TODO: make query safe
		$query = '
		SELECT
			*
		FROM
			`dae_tag`
		WHERE
			`dae_tag`.`id` REGEXP "^('.implode('|',($selected_tags===false?array():$selected_tags)).')$"
		ORDER BY 
			`dae_tag`.`tag_label`
		';
		
		$results = mysqli_query($this->link,$query) or die(mysqli_error($this->link). 'err`d');
		
		while($row = mysqli_fetch_assoc($results) ){
		
			$selected_lnk = $selected_tags;
			foreach($selected_lnk as $k=>$v){
				if($v==$row['id']){
					unset($selected_lnk[$k]);
				}	
			}
			
			$lnk = 'index.php?q=slo/cloud/'.implode('_',$selected_lnk);
		
			$output.= '<li><a href="'.$lnk.'">'.htmlspecialchars($row['tag_label']).'</a></li>';
		}


		$output.= '</ul>
		</div>';
		
		$this->data.= $output;
	}	
	private function tagCloud_content(){
		
		$content = '';
			
		$content .= $this->tagCloud();

		$selected_tags = (empty($this->path[2]) ? false : explode('_',$this->path[2]));
		$slosDisplay = $this->getSlosGivenTags($selected_tags);
		
		if( $selected_tags === false ){
			$selected_tags = array();		

		}

		$content .= '<ul>';		
		foreach($slosDisplay as $slo_id=>$slo_text){

			$slo = SLO::getSLO($slo_id);
			$content.= '<li><a href="index.php?q=slo/'.$slo_id.'">'.$slo->slo_text.'</a></li>';

		}
		
		$content .= '</ul>';

		$this->data.= $content;


		
	}
	private function tagCloud_rightWing(){
		if( ! isset($this->path[2]) || empty($this->path[2])){	
			return;
		}
		$output='<div class="well sidebar-nav">
			<ul class="nav nav-list">
				<li class="nav-header">Refine Search</li>';

		require('tc.php');
		$tags_total = tc($this->link,$this->path[2]);

						
		foreach($tags_total as $tag_arr){		
			if($tag_arr['count'] > 0 ){	
				$output.= '<li><a href="index.php?q=slo/cloud/'.$this->path[2].'_'.$tag_arr['tag_id'].'">'.$tag_arr['label'].'<span class="sub"> ('. $tag_arr['count'] .')</span></a></li>';
			}
		}
			
		$output .= '</ul>
		</div>';

		$this->data.= $output;
	}
	
	
	//helper function for tag cloud. 
	//this function takes some tags, and then gives
	//the learning outcomes taged with all of them
	private function getSlosGivenTags($selected_tags=false,$diequery=false){
		//nothing selected differently
		if( ! isset($this->path[2]) || empty($this->path[2])){
			$ids = array();
			
			$query = '
				SELECT
					`id`,
					`slo_text`
				FROM 
					`dae_slo`
				ORDER BY
					`slo_text`
				';
			$result = mysqli_query($this->link,$query) or die('failed to connect');
			
			while($row = mysqli_fetch_assoc($result)){
				$ids[$row['id']] = $row['slo_text'];
			}
			
			return $ids;
		}
		$queries = array();			
	
	
		$query = '
			SELECT
				`dae_slo_tag`.`slo_id`,
				`dae_slo_tag`.`tag_id`
				#,`dae_slo`.`slo_text` as `slo_text`,
				#COUNT(*) as `times_tagged`
			FROM
				#`dae_slo`  INNER JOIN `dae_slo_tag`
				#	on `dae_slo_tag`.`slo_id` = `dae_slo`.`id`
				`dae_slo_tag`

		';
		
		
		$whereClause = false;
		$wheres = array();
		if( $selected_tags !== false ){
			foreach($selected_tags as $tag_id){
				$whereClause[] = ' `dae_slo_tag`.`slo_id` IN 
				(
				  SELECT 
					`dae_slo_tag`.`slo_id`
					FROM 
						`dae_slo_tag`
					WHERE 
						`dae_slo_tag`.`tag_id` = "'.$tag_id.'"
				)
				
				';
			}
			
			$whereClause = implode(' AND ' ,$whereClause );
		}
		
		if( $whereClause !== false ){
			$query.= 'WHERE' . $whereClause;
		}
		
		$query.= ' 
			GROUP BY 
				`dae_slo_tag`.`id`
			#ORDER BY
			#	`dae_slo`.`slo_text`
		';
			
		
		$result = mysqli_query($this->link,$query)  
			or die('failed to do those things');
		
		$queries[] = $query . ';' . "\n";
		$return = array();
		while($row = mysqli_fetch_assoc($result) ){
			$return[$row['slo_id']] = '$row[\'slo_text\']';
		}
				
				

		return $return;
	
	}
	
	//returns the actual tag cloud.s
	private function tagCloud(){

		$str = '';
		
		require('tc.php');
		
		
		$tags = false;
		
		if( ! (! isset($this->path[2]) || empty($this->path[2])) ){
			$tags = tc($this->link,$this->path[2]);
		}

		if( $tags === false ){
			$tags = array();
						
			$query = '
			SELECT
				`oTag`.`id` as `tag_id`,
				`oTag`.`tag_label`,
				(SELECT COUNT(*) FROM `dae_slo_tag` WHERE `tag_id` = `oTag`.`id` ) as `count`
			FROM
				`dae_tag` as `oTag`
			ORDER BY
				LOWER(`oTag`.`tag_label`)';
			$result = mysqli_query($this->link,$query);
			
			while($row = mysqli_fetch_assoc($result)){
				$tags[] = array(
					'label' => $row['tag_label'],
					'count' => $row['count'],
					'tag_id' => $row['tag_id']
				);
			}
		}
		
		$str .= '
		<div id="tag-cloud">';
							
		//get min and max use for tag usage
		$min_res = mysqli_fetch_assoc(mysqli_query($this->link,'SELECT  COUNT(*) as `count` FROM `dae_slo_tag` GROUP BY `tag_id` order by COUNT(*) ASC'));
		$min = $min_res['count'];
			
		$max_res = mysqli_fetch_assoc(mysqli_query($this->link,'SELECT  COUNT(*) as `count` FROM `dae_slo_tag` GROUP BY `tag_id` order by COUNT(*) DESC'));
		$max_count = $max_res['count'];
						
		$min = 0 ; $max = 1;

		foreach($tags as $data){
			if( $data['count'] > 0 ){
				$query = 'SELECT  COUNT(*) as `count` FROM `dae_slo_tag` WHERE `tag_id` = "'.$data['tag_id'].'"'; 
				$result = mysqli_fetch_assoc(mysqli_query($this->link,$query));
				$count = $result['count'];
				//http://stackoverflow.com/questions/3717314/what-is-the-formula-to-calculate-the-font-size-for-tags-in-a-tagcloud
			
				$log = true;
				
				if( $log === true ){
					$percent = log($count) / log($max_count) *($max-$min)+$min;
					$percent *= 100;
				}
				else{
					$percent = ($count) / ($max_count) *($max-$min)+$min;
					$percent *= 100;
				}

				if( $percent >= 80 ){
					$bucket = '4';
				}
				else if( $percent >=70 ){
					$bucket = '3';
				}
				else if( $percent >= 50 ){
					$bucket = '2';
				}
				else if( $percent >= 30 ){
					$bucket = '1';
				}
				else{
					$bucket = '0';
				}
	
				$str.= '<span class="badge badge-success tag cloud-'.$bucket.'">';
				
				$link = 'index.php?q=slo/cloud/'.$data['tag_id'];
				if( isset($this->path[2]) && ! empty($this->path[2]) ){
					$link.= '_'.$this->path[2];
				}
				$str.= '<a href="'.$link.'">'.$data['label'].'</a>';
				
				$str.= '</span> ';
			}
		
		}
	
		$str.= '
		</div>';		
		return $str ;
	}
	
	
}
