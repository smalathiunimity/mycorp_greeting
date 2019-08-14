<?php

function greeting_services_resource() {

  module_load_include('inc', 'services', 'resources/node_resource');
		
  $api = array(
	'greeting' => array(
  	'operations' => array(
    	'retrieve' => array(
			'help' => 'Retrieves greeting node info.',
			'file' => array(
				'type' => 'inc',
				'module' => 'greeting',
				'name' => 'includes/greeting.services',
			),
			'callback' => '_greeting_resource_retrieve',
			'access callback' => '_greeting_node_access',
			'access arguments' => array('access content'),
			'access arguments append' => FALSE,
			'args' => array(
				array(
				  'name' => 'nid',
				  'optional' => FALSE,
				  'source' => array('path' => 0),
				  'type' => 'int',
				  'description' => 'The nid of the node to retrieve',
				),
			),
    	),
		'create' => array(
          'help' => 'Create Greeting Node.',
          'callback' => '_greeting_node_create',
          'access callback' => 'user_access',
          'access arguments' => array('access content'),
          'access arguments append' => FALSE,
          'args' => array(
            array(
              'name' => 'node',
              'type' => 'array',
              'description' => 'Create greeting node.',
              'source' => 'data',
              'optional' => FALSE,
            ),
          ),
        ),
        'update' => array(
          'help' => 'Update a node',
          'file' => array(
				'type' => 'inc',
				'module' => 'greeting',
				'name' => 'includes/greeting.services',
			),
          'callback' => '_greeting_node_update',
          'args' => array(
            array(
              'name' => 'nid',
              'optional' => FALSE,
              'source' => array('path' => 0),
              'type' => 'int',
              'description' => 'The nid of the node to update',
            ),            
			array(
              'name' => 'state',
              'optional' => TRUE,
              'source' => array('path' => 1),
              'type' => 'string',
              'description' => 'The nid of the node to update',
            ),
			array(
              'name' => 'node',
              'optional' => TRUE,
              'source' => 'data',
              'description' => 'The node data to update',
              'type' => 'array',
            ),
          ),
          'access callback' => 'user_access',
          'access arguments' => array('access content'),
          'access arguments append' => FALSE,
        ),
        'delete' => array(
          'help' => t('Delete a node'),
          'file' => array(
				'type' => 'inc',
				'module' => 'greeting',
				'name' => 'includes/greeting.services',
		   ),
          'callback' => '_greeting_node_delete',
          'args' => array(
            array(
              'name' => 'nid',
              'optional' => FALSE,
              'source' => array('path' => 0),
              'type' => 'int',
              'description' => 'The nid of the node to delete',
            ),
          ),
          'access callback' => 'user_access',
          'access arguments' => array('access content'),
          'access arguments append' => FALSE,
        ),
	  ),
	),
  );

  return $api;
}
/**
 * [_greeting_resource_retrieve] definition.
 * Returns the information about a greeting.
 * @param $nid
 *   The nid of the greeting node.
 * @return array
 *	The node processed information array.
 */
function _greeting_resource_retrieve($nid) {
	$return_obj = array();
	if($nid == 'retrieve') {
		//build db query
		$result = db_select('node', 'n')
					->fields('n')->condition('status', 1)
					->condition("type","greeting", '=')
					->execute();

        //fetch each node from database
         foreach ($result as $record) {
            $node = node_load($record->nid);		
			//user has update permission to this node
			$return_obj[] = array(
			  'title' => $node->title,
			  'id' => $node->nid,
			  'image' => get_image_url($node->field_image['und'][0]['uri']),
			  'created_date' => $node->created,			  
			  'status' => $node->status,
			);
		}
	} else {	
		$node = node_load($nid);
		if($node && $node->type == 'greeting') {
			//Create return object.
			$return_obj = array(
			  'title' => $node->title,
			  'id' => $node->nid,
			  'image' => get_image_url($node->field_image['und'][0]['uri']),
			  'created_date' => $node->created,
			  'status' => $node->status,
			);
		} else {
			//Create return object.
			$return_obj = array(
			  'message' => 'No Result Found.'
			);
		}		
	}
	return $return_obj;
}

/**
* [_greeting_node_create] definition.
* Creates a new node based on submitted values.
*/
function _greeting_node_create($node) {
	
	if ($node['type'] == 'greeting' ||  !$node['type']) {		
		return _node_resource_create($node);
	} else {
		return services_error(t("This node type @type can't be processed via services", array('@type' => $node['type'])), 406);
	}	
}

/**
* [_greeting_node_update] definition.
* Update a node based on submitted values.
*/
function _greeting_node_update($nid, $state, $node) {
	$info = node_load($nid);
	if ($info && $info->type == 'greeting') {
		if(in_array($state,array('publish', 'unpublish'))) {
			$status = ($state == "publish")? 1 : 0;
			// load the node object
			$nodeUpdate = node_load($nid);
			$nodeUpdate->status = $status;
			if(node_save($nodeUpdate))
				return false;
			else 
				return true;
		} else {					
			return _node_resource_update($nid, $node);			
		}
	} else {
		return services_error(t("This node type @type can't be processed via services", array('@type' => $node['type'])), 406);
	}
}


/**
* [_greeting_node_delete] definition.
* Delete a node given its nid.
*/
function _greeting_node_delete($nid) {
	
	$info = node_load($nid);
	
	if ($info->type == 'greeting') {		
		return _node_resource_delete($nid);
	} else {
		return services_error(t("This node type @type can't be processed via services", array('@type' => $node['type'])), 406);
	}	
}

/**
* [get_image_url] definition.
* Determine the image url.
*/
function get_image_url($uri){	
	if($uri){
		$image_uri = file_create_url($uri);
	} else {
		$image_uri = '';
	}
	return $image_uri;
}

/**
* [_greeting_node_access] definition.
* Determine whether the current user can access a node resource.
*/
function _greeting_node_access(){
	global $user;
	if (user_is_logged_in()) {		
		return true;
	}
}
