/*
 * A custom chart example based on the d3chartview of the web framework.
 * The component is initialized in Javascript which provides more flexibility and formatting options.
 */
require([
    'underscore',
    'jquery',
    'splunkjs/mvc/utils',
    'splunkjs/mvc',
    'splunkjs/mvc/d3chart/d3chartview',
    'util/moment',
    'splunkjs/mvc/simplexml/ready!'
], function(_, $, utils, mvc, D3ChartView, moment) {

    // Re-use the search manager of the results table
    var tableManagerid = mvc.Components.get('table1').settings.get('managerid');


    var dropdown1 = mvc.Components.getInstance("field2");


    // Create the D3 chart view
    var chart = new D3ChartView({
        "id": "chart1",
        "managerid": tableManagerid,
        "type": "discreteBarChart",
        "el": $('#chart1')
    },{tokens: true}).render();

    var mySearch = mvc.Components.getInstance('search1'); //find the search component
    var myResults = mySearch.data("results", {count:0}); //get all the results
    var chartTicks = [];

    var tokens = mvc.Components.getInstance("default");

    myResults.on("data", function() {
        // var numResults = myResults.data().rows.length;
        var myData = myResults.data().rows;

        var index = 0;
        _.each(myData, function(myDatum, i) {
            if ((i+1) % 8 == 0 || i == 0){ //get the first and every 8th
                chartTicks[index] = myDatum[0];
                index++;
            }
        });


        chart.settings.set("setup", function(chart) {
            // Configure chart formatting
            chart.color(d3.scale.category10().range());
            chart.margin({right: 100, left: 80, top: 70, bottom: 70 });

            // Format the X-Axis labels
            chart.xAxis.tickValues(chartTicks);  //pass in the values you want to show

            chart.xAxis.rotateLabels(-45);
            chart.xAxis.axisLabel(tokens.get("context"));
            chart.yAxis.axisLabel("Membership");
        });
    });
});
