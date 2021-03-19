/// <reference path="../../js/lib/angular.min.js" />
/// <reference path="../../js/TestReportController.js" />
/// <reference path="../../bower_components/jasmine-core/lib/jasmine-core/jasmine.js" /> 
/// <reference path="../../bower_components/jasmine-core/lib/jasmine-core/boot.js" /> 
/// <reference path="../../bower_components/angular-mocks/angular-mocks.js" /> 
(function () {
		describe('TestReportController tests', function () {
			var scope;
			var controller;
			beforeEach(function () {
				module ('testReportApp');
			});

			beforeEach(inject(function ($rootScope, $controller) {
				scope = $rootScope.$new();
				controller = $controller ('TestReportController', {'$scope': scope, 'data' : {}} );
			}));

			it('suiteCommandLine builds correct command for native suite', function () {
				var suite = {'name': 'native'};
				var commandLine = scope.suiteCommandLine (suite);
				expect(commandLine).toBe('perl ./Tools/UnifiedTestRunner/test.pl --suite=native');
			});

			it('suiteCommandLine builds correct command for runtime suite', function () {
				var suite = {'name': 'runtime'};
				var commandLine = scope.suiteCommandLine (suite);
				expect(commandLine).toBe('perl ./Tools/UnifiedTestRunner/test.pl --suite=runtime');
			});

			it('suiteCommandLine builds correct command for runtime suite', function () {
				var suite = {'name': 'runtime', 'minimalCommandLine': ['--platform=standalone']};
				var commandLine = scope.suiteCommandLine (suite);
				expect(commandLine).toBe('perl ./Tools/UnifiedTestRunner/test.pl --suite=runtime --platform=standalone');
			});

			it('testCommandLine builds correct command for runtime test', function () {
				var suite = {'name': 'runtime', 'minimalCommandLine': ['--platform=standalone']};
				var test = {'name': 'test1'};
				var commandLine = scope.testCommandLine (suite, test);
				expect(commandLine).toBe('perl ./Tools/UnifiedTestRunner/test.pl --suite=runtime --platform=standalone --testfilter=test1');
			});
		});
})();

