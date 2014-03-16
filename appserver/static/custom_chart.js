/*
 * A custom chart example based on the d3chartview of the web framework.
 * The component is initialized in Javascript which provides more flexibility and formatting options.
 */
require([
    'underscore',
    'jquery',
    'splunkjs/mvc/utils',
    'splunkjs/mvc',
    //manually load direct dependencies
    'splunkjs/mvc/d3chart/d3/d3.v2',
    'splunkjs/mvc/d3chart/d3/nv.d3',
    // original d3 wrapper
    // 'splunkjs/mvc/d3chart/d3chartview', 
    'util/moment',
    'splunkjs/mvc/simplexml/ready!'
], function(_, $, utils, mvc, d3_v2, nv_d3, moment) {

    // Re-use the search manager of the results table
    /* removing tableManagerid as it is not used in nv graphing */
    // var tableManagerid = mvc.Components.get('table1').settings.get('managerid');

    /* not used */
    // var dropdown1 = mvc.Components.getInstance("field2");

    var mySearch = mvc.Components.getInstance('search1'); //find the search component
    var myResults = mySearch.data("results", {count:0}); //get all the results
    var chartTicks = [];

        require(["https://raw.github.com/novus/nvd3/master/nv.d3.min.js"],function() {

    /* These lines are all chart setup.  Pick and choose which chart features you want to utilize. */
    nv.addGraph(function() {

        var chart = nv.models.lineChart()
            .margin({left: 100})            // Adjust chart margins to give the x-axis some breathing room.
            .useInteractiveGuideline(true)  // We want nice looking tooltips and a guideline!
            .transitionDuration(600)        // how fast do you want the lines to transition?
            .showLegend(true)               // Show the legend, allowing users to turn on/off line series.
            .showYAxis(true)                // Show the y-axis
            .showXAxis(true);               // Show the x-axis

    
        var tokens = mvc.Components.getInstance("default");

        chart.xAxis     //Chart x-axis settings
            .rotateLabels(-45)
            .axisLabel(tokens.get("context"));

        chart.yAxis     //Chart y-axis settings
            .axisLabel('Membership');

        myResults.on("data", function() {
            var myData = myResults.data().rows;
            var index = 0;

            /* get terms from tokens for key at top of chart */
            var labels = tokens.get("terms") ? tokens.get("terms").split(",") : myResults.data().fields.splice(1,myResults.data().fields.length);
            /* 
            ** had to reformat data for graph 
            ** original was an array of arrays each with values for context terms
            ** transform to array of objects that represent series (line), x and y values) -- could be improved
            ** (group data points by lines)
            */
            var values = [];
            var obj_data = [];
            
            _.each(myData, function(myDatum, i) {
                //0 = x axis
                var x_val = 0;
                _.each(myDatum, function(data_point, r){
                    if(r == 0){
                        x_val = data_point; //this is the position on x-axis for the following number of y values
                        return;
                    }
                    if ((i+1) % 8 == 0 || i == 0){ //get the first and every 8th
                        chartTicks[index] = x_val;
                        index++;
                    }
                    if(r-1 == values.length){
                        values[r-1] = [];
                    }

                    /* in this series (line) push this data point */
                    values[r-1].push({
                        series: r-i, // may not be needed
                        x: parseFloat(x_val), 
                        y: parseFloat(data_point)
                    });
                });
            });
            
            /* nv.d3 expects an array of objects, one object for each series (line) */
            _.each(values, function(line, i){
                obj_data.push({
                    values: line,
                    key: labels[i],
                    area: true
                });
            });

            chart.xAxis.tickValues(chartTicks);  //pass in the values you want to show

            /* 
            ** d3 wants to be addressed to an svg element.
            ** the current template does not contian an svg element
            ** we create and append an svg element with d3 before loading data
            */
            d3.select('#chart1 svg')    //Select the <svg> element you want to render the chart in.   
                .datum(obj_data)         //Populate the <svg> element with chart data...
                .call(chart);
         });
        
        // append an svg element to the chart for addressing when loading (only if doesn't already exist!)
        if(!$('#chart svg').length > 0){
            var _svg = d3.select('#chart1').append('svg');    
        }
        
        /* resize listener to update chart to fit in available space */
        nv.utils.windowResize(function() { chart.update() });

        return chart;
        });
    });


});
