var faceApp = angular.module('faceApp', ['ngResource']);

faceApp.factory ('Suites', function ($resource) { return $resource ('http://localhost:3000/suites') } )

faceApp.controller('SuitesController', function ($scope, Suites) {
    $scope.msg = Suites.query ();
});